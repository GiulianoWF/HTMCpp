#pragma once

#include <boost/beast/websocket.hpp>
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
    struct RouteEntry {
        std::function<std::string const()> executor;
        std::string contentType;
        std::optional<std::function<void(http::response<http::string_body>&)>> modifier;
    };
    std::unordered_map<std::string, RouteEntry> mRoutes;
    public:
        virtual void HandleHttpRequest (http::request<http::string_body>&& req ,
                                        std::function<void(http::response<http::string_body>)>&& sendCallback);

        void AppendHandler (std::string&& route, std::function<std::string const()>&& handler, std::string&& contentType="text/html");

        void AppendFile (std::string&& route, std::string&& filePath, std::string&& contentType="text/html");

        void AppendResponseModifiers (std::string&& route, std::function<void(http::response<http::string_body>&)>);

    private:
        auto ReadFileToString(std::string&& filename) ->std::string;
};

auto GetServerHandler() -> HtmxHandler&;
