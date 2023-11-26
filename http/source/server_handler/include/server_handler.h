#pragma once

#include <boost/beast/http/verb.hpp>
#include <boost/beast/websocket.hpp>
#include <optional>
#include <queue>
#include <string>
#include <unordered_set>
#include <mutex>
#include <iostream>

namespace http = boost::beast::http;

class ServerWebsocktSession;
class ServerHandler {
    private:
        std::mutex mSessionsMutex;
        std::unordered_set<ServerWebsocktSession*> mAllSessions;
        std::unordered_multimap<std::string, ServerWebsocktSession*> mTopicsMap;
        int mDebugInt = 0;

    public:
        ServerHandler();

        virtual void HandleHttpRequest (http::request<http::string_body>&& req , std::function<void(http::response<http::string_body>)>&& sendCallback);
        virtual void HandleWebsocketMessage (ServerWebsocktSession* session, std::string const& message);
        virtual void Add (ServerWebsocktSession* session);
        virtual void Leave (ServerWebsocktSession* session);
        virtual void Broadcast (std::string message);
};

class HtmxHandler : public ServerHandler
{
    public:
        struct Route {
            std::string mPath;
            boost::beast::http::verb mVerb;
            bool operator==(const Route& rhs) const;
        };

    private:
        struct RouteEntry {
            std::function<std::string const(std::vector<std::string>&&)> executor;
            std::string contentType;
            std::optional<std::function<void(http::response<http::string_body>&)>> modifier;
        };

        struct TrieNode {
            std::string sequence;
            std::vector<std::pair<Route, std::unique_ptr<TrieNode>>> nextSequences;
            std::unique_ptr<TrieNode> nextSequenceArgEntry;
            std::unique_ptr<HtmxHandler::RouteEntry> route;
        };

        std::unique_ptr<TrieNode> mRoutesRoot = nullptr;

    public:
        HtmxHandler();

        virtual void HandleHttpRequest (http::request<http::string_body>&& req ,
                                        std::function<void(http::response<http::string_body>)>&& sendCallback);

        void AppendHandler (Route&& route,
                            std::function<std::string const(std::vector<std::string>&&)>&& handler,
                            std::string&& contentType="text/html");

        void AppendFile (Route&& route,
                         std::string&& filePath,
                         std::string&& contentType="text/html");

        void AppendResponseModifiers (Route&& route, std::function<void(http::response<http::string_body>&)>);

    private:
        auto TokenizeRoute(Route&& route) ->std::queue<Route>;
        void RegisterRoute(Route&& route, RouteEntry&& routeEntry);
        auto ResolveRoute(Route&& route) ->std::tuple<RouteEntry*, std::vector<std::string>>;
        auto ReadFileToString(std::string&& filename) ->std::string;
        auto Transverse(TrieNode* root, Route&& route) ->std::tuple<bool, std::vector<std::string>, RouteEntry*>;
        auto Insert(TrieNode* root, Route&& route, RouteEntry routeEntry) ->bool;
        auto CreateRouteNodeChain(TrieNode* root, std::queue<Route>&& routeTokens, RouteEntry routeEntry) ->bool;
        auto CreateRouteNode(TrieNode* root, Route&& routeToken, RouteEntry routeEntry) ->bool;
};

auto GetServerHandler() -> HtmxHandler&;
