#include "server.h"
#include "server_handler.h"

#include <db_issues.hpp>
#include "view_issues.hpp"

int main(int argc, char* argv[])
{
    auto handler = std::make_shared<HtmxHandler>();

    handler->AppendFile("/", "./home.html");

    handler->AppendFile("/style.scss", "style.scss", "text/css");

    handler->AppendHandler("/issues", []()->std::string{
        return GetIssueListComponent();
    });

    RunServer(argc, argv, std::dynamic_pointer_cast<ServerHandler>(handler));
}
