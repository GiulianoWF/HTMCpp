#include "server.h"
#include "server_handler.h"

#include <boost/beast/core.hpp>

#include <sstream>
#include <iostream>

class HtmxHandler : public ServerHandler
{
    public:
        virtual void HandleHttpRequest (http::request<http::string_body>&& req , std::function<void(http::response<http::string_body>)>&& sendCallback)
        {
            std::stringstream ss;
            if (req.target() == "/") {
                ss << R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Flask HTMX Example</title>
    <!-- Include HTMX library -->
    <script src="https://unpkg.com/htmx.org@1.6.1"></script>
</head>
<body>
    <div id="content">
        <button hx-get="/route2"
                hx-target="#content"
                hx-swap="outerHTML">
            Load Content
        </button>
    </div>
</body>
</html>
                )";
            } else if (req.target() == "/route2") {
                ss << "<div>Hello!</div>";
            } else {
                ss << "error";
            }
            req.set("Header1", "header content here");
            //ss << mDebugInt++ << " Unknown HTTP-method";
            std::cout << req.method_string() << std::endl;
            std::cout << (req.target() == "/route2") << std::endl;
                
            http::response<http::string_body> res{http::status::ok, req.version()};
            //res.set(http::field::server, "[ServerHandler]");
            res.set(http::field::content_type, "text/html");
            res.keep_alive(false);
            res.body() = ss.str();
            res.prepare_payload();

            sendCallback(res);
        }

};

int main(int argc, char* argv[])
{
    boost::asio::ip::address address;
    unsigned short port;

    if (argc != 3) {
        std::cout << "You can pass server ip and port as arguments.\n Example:\n .\\activity_manager.exe \"192.168.0.17\" \"9091\"";
        std::cout << "\nWhat is the address the Activity Manager is hosted on? (ex.: 0.0.0.0)\n";
        std::string userAddress;
        std::cin >> userAddress;
        address = boost::asio::ip::make_address(userAddress.c_str());

        std::cout << "What is should be the Activity Manager port? (ex.: 8080)\n";
        std::cin >> port;
    } else {
        address = boost::asio::ip::make_address(argv[1]);
        port = static_cast<unsigned short>(std::atoi(argv[2]));
    }

    boost::asio::io_context ioContext;

    auto server = std::make_shared<Server>(ioContext, boost::asio::ip::tcp::endpoint{address, port}, std::make_shared<HtmxHandler>());
    server->Run();

    ioContext.run();
}
