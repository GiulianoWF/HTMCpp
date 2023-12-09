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
    handler->AppendHandler({"/", boost::beast::http::verb::get}, [](std::vector<std::string>&& a, std::unordered_map<std::string, std::string>)->std::string{
        return GetLandpage();
    });

    handler->AppendFile({"/style.scss", boost::beast::http::verb::get}, "style.scss", "text/css");
    handler->AppendFile({"/issues_table.scss", boost::beast::http::verb::get}, "issues_table.scss", "text/css");

    handler->AppendHandler({"/issues", boost::beast::http::verb::get}, [](std::vector<std::string>&& a, std::unordered_map<std::string, std::string>)->std::string{
        return GetIssueListComponent();
    });

    handler->AppendHandler({"/issues/$", boost::beast::http::verb::delete_}, [](std::vector<std::string>&& args, std::unordered_map<std::string, std::string>)->std::string{
        std::stringstream ss(args.at(0));
        int64_t issue_id;
        if (ss >> issue_id) {
            Database::DeleteIssue(issue_id);
        }
        return "";
    });

    handler->AppendHandler({"/create-issue", boost::beast::http::verb::post}, [](std::vector<std::string>&& a, std::unordered_map<std::string, std::string> b)->std::string{
        int64_t issueId;
        int64_t colaboratorId;
        try {
            issueId = std::stoi(b["issue_id"]);
            colaboratorId = std::stoi(b["colaborator_id"]);
        } catch (...) {
            return "";
        }

        Database::SaveIssue({Database::Issue(
            0,
            issueId,
            b["description"],
            colaboratorId,
            1
        )});

        return "";
    });
    
    RunServer(argc, argv, std::dynamic_pointer_cast<ServerHandler>(handler));
}
  
