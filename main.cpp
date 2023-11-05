#include "server.h"
#include "server_handler.h"

#include <db_issues.hpp>
#include "view_issues.hpp"

int main(int argc, char* argv[])
{
    auto handler = std::make_shared<HtmxHandler>();

    handler->AppendFile({"/", boost::beast::http::verb::get}, "./home.html");

    handler->AppendFile({"/style.scss", boost::beast::http::verb::get}, "style.scss", "text/css");

    handler->AppendHandler({"/issues", boost::beast::http::verb::get}, [](std::vector<std::string>&& a)->std::string{
        return GetIssueListComponent();
    });

    handler->AppendHandler({"/issues/$", boost::beast::http::verb::delete_}, [](std::vector<std::string>&& a)->std::string{
        std::cout << "Delete " << a.at(0) << std::endl;
        return "";
    });

    RunServer(argc, argv, std::dynamic_pointer_cast<ServerHandler>(handler));
}
