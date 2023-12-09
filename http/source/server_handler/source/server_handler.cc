#include "server_handler.h"
#include "server_websocket_session.h"
#include <complex>
#include <fstream>
#include <memory>
#include <queue>
#include <set>

#include <string>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <iomanip>

constexpr size_t maxVariableLength = 100;

auto GetServerHandler() -> HtmxHandler& {
    static HtmxHandler token_;
    return token_;
}

ServerHandler::ServerHandler() {
}

void ServerHandler::HandleHttpRequest(http::request<http::string_body>&& req , std::function<void(http::response<http::string_body>)>&& sendCallback) {
    std::stringstream ss;
    ss << mDebugInt++ << " Unknown HTTP-method";

    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, "[ServerHandler]");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = ss.str();
    res.prepare_payload();

    sendCallback(res);
}

void ServerHandler::HandleWebsocketMessage(ServerWebsocktSession * session, std::string const& message) {
    std::cout << "Recieve websocket message from " << session << std::endl;
    std::cout << "Message is " << message << std::endl;
    this->Broadcast(message);
}

void ServerHandler::Add(ServerWebsocktSession * session) {
    std::lock_guard<std::mutex> lock(this->mSessionsMutex);
    std::cout << "Adding Websocket session " << session << std::endl;
    this->mAllSessions.insert(session);
}

void ServerHandler::Leave(ServerWebsocktSession * session) {
    {
        std::lock_guard<std::mutex> lock(this->mSessionsMutex);
        std::cout << "Removing Websocket session " << session << std::endl;
        auto position = this->mAllSessions.find(session);
        if(position != this->mAllSessions.end()) {
            this->mAllSessions.erase(position);
        }
    }
    this->Broadcast("Colegue leaved");
}

void ServerHandler::Broadcast(std::string message) {
    std::cout << "Broadcasting " << message << std::endl;

    auto const ss = std::make_shared<std::string const>(std::move(message));

    std::vector<std::weak_ptr<ServerWebsocktSession>> sessionsWeakPtrs;
    {
        std::lock_guard<std::mutex> lock(this->mSessionsMutex);

        sessionsWeakPtrs.reserve(this->mAllSessions.size());
        for(auto serverWebsocketSession : this->mAllSessions) {
            sessionsWeakPtrs.emplace_back(serverWebsocketSession->weak_from_this());
        }
    }

    for(auto const& sessionWkPtr : sessionsWeakPtrs) {
        if(auto sessionsPtr = sessionWkPtr.lock()) {
            sessionsPtr->Send(ss);
        }
    }
}

bool HtmxHandler::Route::operator==(const Route& rhs) const
{
    return this->mPath == rhs.mPath && this->mVerb == rhs.mVerb;
}

HtmxHandler::HtmxHandler()
    :ServerHandler()
{
    this->mRoutesRoot = std::unique_ptr<TrieNode>(new TrieNode{
        .sequence = "",
        .nextSequences = {},
        .nextSequenceArgEntry = nullptr,
        .route = nullptr
    });
}

std::string urlDecode(const std::string& str) {
    std::string result;
    char ch;
    int i, j;

    for (i = 0; i < str.length(); i++) {
        if (int(str[i]) == 37) {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &j);
            ch = static_cast<char>(j);
            result += ch;
            i = i + 2;
        } else {
            result += str[i];
        }
    }

    return result;
}

std::unordered_map<std::string, std::string> parseQueryString(const std::string& query) {
    // TODO: safaty, sanityze, etc :(
    std::unordered_map<std::string, std::string> data;
    std::istringstream queryStream(query);
    std::string pair;

    while (std::getline(queryStream, pair, '&')) {
        auto delimiterPos = pair.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = pair.substr(0, delimiterPos);
            std::string value = urlDecode(pair.substr(delimiterPos + 1));
            data.insert(std::make_pair(key, value));
        }
    }

    return data;
}

void HtmxHandler::HandleHttpRequest (http::request<http::string_body>&& req , std::function<void(http::response<http::string_body>)>&& sendCallback)
{
    auto [pRoute, parsedArgs] = this->ResolveRoute({req.target(), req.method()});

    if(pRoute == nullptr) {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.prepare_payload();
        sendCallback(res);
        return;
    }
    auto method = req.method();

    const std::string&& routeResponse = pRoute->executor(std::move(parsedArgs), parseQueryString(req.body()));

    std::stringstream ss;
    ss << std::move(routeResponse);

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type,  pRoute->contentType);

    if (pRoute->modifier.has_value()) {
        pRoute->modifier.value()(res);
    } else {
        res.keep_alive(false);
    }

    res.body() = ss.str();
    res.prepare_payload();

    sendCallback(res);
}

void HtmxHandler::AppendHandler (Route&& route,
                                 ExecuterSignature&& handler,
                                 std::string&& contentType) {
    this->RegisterRoute(std::move(route),{
        .executor = handler,
        .contentType = contentType,
        .modifier = std::nullopt
    });
}

void HtmxHandler::AppendFile (Route&& route,
                              std::string&& filePath,
                              std::string&& contentType) {
    const std::string& content = this->ReadFileToString(std::move(filePath));
    if (content.empty()) {
        throw "File not found";
    }

    const auto handler = [content](std::vector<std::string>&&, std::unordered_map<std::string, std::string>) -> std::string
    {
        return content;
    };

    this->RegisterRoute(std::move(route),{
        .executor = handler,
        .contentType = contentType,
        .modifier = std::nullopt
    });
}

void HtmxHandler::AppendResponseModifiers (Route&& route, std::function<void(http::response<http::string_body>&)> modifier) {
    auto [pRoute, parsedArgs] = this->ResolveRoute(std::move(route));
    if(pRoute == nullptr) {
        return;
    }
    pRoute->modifier = modifier;
}

auto HtmxHandler::TokenizeRoute(Route&& route) ->std::queue<Route> {
    std::string const& routeString = route.mPath;

    size_t pos_start = 0, pos_end, delim_len = 1;
    std::string token;
    std::queue<Route> res;

    while ((pos_end = routeString.find("/", pos_start)) != std::string::npos) {
        token = routeString.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;

        if(token == "") {
            continue;
        }

        res.push({token, route.mVerb});
    }

    auto lastString = routeString.substr(pos_start);
    if (lastString != "") {
        res.push({lastString, route.mVerb});
    }

    return res;
}

void HtmxHandler::RegisterRoute(Route&& path, RouteEntry&& route) {
    this->Insert(this->mRoutesRoot.get(), std::move(path), route);
}

auto HtmxHandler::ResolveRoute(Route&& path) ->std::tuple<RouteEntry*, std::vector<std::string>> {
    auto [found, args, foundRoute] = this->Transverse(this->mRoutesRoot.get(), std::move(path));

    return {foundRoute, args};
}

auto HtmxHandler::ReadFileToString(std::string&& filename) ->std::string {
    std::ifstream ifs("resources/" + filename);

    if (!ifs) {
        throw "Resource not found";
    }

    std::stringstream ss;
    ss << ifs.rdbuf();

    return ss.str();
}

auto HtmxHandler::Transverse(TrieNode* root, Route&& route) ->std::tuple<bool, std::vector<std::string>, RouteEntry*> {
    std::vector<std::string> parsedArgs;
    
    std::queue<Route> tokens = this->TokenizeRoute(std::move(route));

    if(tokens.empty()) {
        if(root->route == nullptr) {
            return {false, {}, nullptr};
        }

        return {true, {}, root->route.get()};
    }

    auto currentToken = tokens.front();

    TrieNode* currentNode = root;

    LOOP_ROUTES:
    while(!tokens.empty()) {
        for(auto& sequence : currentNode->nextSequences) {
            if (sequence.first == currentToken) {
                currentNode = sequence.second.get();
                tokens.pop();
                if(!tokens.empty()) {
                    currentToken = tokens.front();
                }
                goto LOOP_ROUTES;
            }
        }

        if (currentNode->nextSequenceArgEntry != nullptr) {
            parsedArgs.push_back(currentToken.mPath);
            currentNode = currentNode->nextSequenceArgEntry.get();
            tokens.pop();
            if(tokens.empty()) {
               break; 
            }
            currentToken = tokens.front();
            continue;
        }

        return {false, {}, nullptr};
    }

    return {true, parsedArgs, currentNode->route.get()};
}

auto HtmxHandler::Insert(TrieNode* root, Route&& route, RouteEntry routeEntry) ->bool {
    std::queue<Route> tokens = this->TokenizeRoute(std::move(route));

    if(tokens.empty()) {
        auto heapRoute = std::unique_ptr<RouteEntry>(new RouteEntry);
        *(heapRoute.get()) = routeEntry;
        root->route = std::move(heapRoute);
        return true;
    }

    auto currentToken = tokens.front();

    TrieNode* currentNode = root;

    LOOP_ROUTES:
    while(!tokens.empty()) {
        for(auto& sequence : currentNode->nextSequences) {
            if (sequence.first == currentToken) {
                currentNode = sequence.second.get();
                tokens.pop();
                currentToken = tokens.front();
                goto LOOP_ROUTES;
            }
        }

        if (currentNode->nextSequenceArgEntry != nullptr
            && currentToken.mPath == "$") {
            currentNode = currentNode->nextSequenceArgEntry.get();
            tokens.pop();
            currentToken = tokens.front();
            continue;
        }

        this->CreateRouteNodeChain(currentNode, std::move(tokens), routeEntry);
        return true;
    }

    // Route already exist
    return false;
}

auto HtmxHandler::CreateRouteNodeChain(TrieNode* root, std::queue<Route>&& routeTokens, RouteEntry routeEntry) ->bool {
    if (routeTokens.empty()) {
        return false;
    }

    auto currentToken = routeTokens.front();
    routeTokens.pop();

    TrieNode* currentNode = root;
    TrieNode* nextNode = currentNode;

    while(routeTokens.size() >= 1) {
        auto blankNode = std::unique_ptr<TrieNode>(new TrieNode{
            "",
            {},
            {},
            {}
        });

        nextNode = blankNode.get();

        if (currentToken.mPath == "$") {
            currentNode->nextSequenceArgEntry = std::move(blankNode);
        } else {
            currentNode->nextSequences.push_back({currentToken, std::move(blankNode)});
        }

        currentNode = nextNode;

        currentToken = routeTokens.front();
        routeTokens.pop();
    }

    this->CreateRouteNode(currentNode, std::move(currentToken), routeEntry);

    return true;
}

auto HtmxHandler::CreateRouteNode(TrieNode* root, Route&& routeToken, RouteEntry routeEntry) ->bool {
    if (routeToken.mPath.empty()) {
        return false;
    }

    RouteEntry* routeCopy = new RouteEntry;
    *routeCopy = routeEntry;

    auto node = std::unique_ptr<TrieNode>(new TrieNode{
        "",
        std::vector<std::pair<Route, std::unique_ptr<TrieNode>>>{},
        std::unique_ptr<TrieNode>(nullptr),
        std::unique_ptr<RouteEntry>(routeCopy)
    });

    if (routeToken.mPath == "$") {
        root->nextSequenceArgEntry = std::move(node);
    } else {
        root->nextSequences.push_back({routeToken, std::move(node)});
    }

    return true;
}
