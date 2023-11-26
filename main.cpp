#include "server.h"
#include "server_handler.h"

#include <db_issues.hpp>
#include "view_issues.hpp"
#include "view_landpage.hpp"

#include <sstream>

int main(int argc, char* argv[])
{
    auto handler = std::make_shared<HtmxHandler>();

    // handler->AppendFile({"/", boost::beast::http::verb::get}, "./home.html");
    handler->AppendHandler({"/", boost::beast::http::verb::get}, [](std::vector<std::string>&& a)->std::string{
        return GetLandpage();
    });

    handler->AppendFile({"/style.scss", boost::beast::http::verb::get}, "style.scss", "text/css");
    handler->AppendFile({"/issues_table.scss", boost::beast::http::verb::get}, "issues_table.scss", "text/css");

    handler->AppendHandler({"/issues", boost::beast::http::verb::get}, [](std::vector<std::string>&& a)->std::string{
        return GetIssueListComponent();
    });

    handler->AppendHandler({"/issues/$", boost::beast::http::verb::delete_}, [](std::vector<std::string>&& args)->std::string{
        std::stringstream ss(args.at(0));
        int64_t issue_id;
        if (ss >> issue_id) {
            Database::DeleteIssue(issue_id);
        }
        return "";
    });
    
    RunServer(argc, argv, std::dynamic_pointer_cast<ServerHandler>(handler));
}
  
