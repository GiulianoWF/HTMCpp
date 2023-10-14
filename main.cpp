#include "server.h"
#include "server_handler.h"

#include <issues.hpp>
#include "components/issues.hpp"

int main(int argc, char* argv[])
{
    auto handler = std::make_shared<HtmxHandler>();
    handler->AppendFile("/", "./home.html");
    handler->AppendHandler("/issues", []()->std::string{
        std::vector<Database::Issues> issues = Database::GetAllIssues();
        if (issues.empty()) {
            return "<div>No Issues!</div>";
        } else {
            return GetIssueListComponent();
        }
    });

    RunServer(argc, argv, std::dynamic_pointer_cast<ServerHandler>(handler));
}
