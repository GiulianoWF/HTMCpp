#include <db_issues.hpp>
#include <db_database_connection.hpp>

namespace Database
{
    auto GetAllIssues() ->std::vector<Issue> {
        DatabaseConnection& database = GetDatabase();
        auto liveQuery = database.ExecuteQuery("SELECT creation_timestamp, issue_id, description, colaborator_id, state FROM ISSUES;");

        std::vector<Issue> issues;
        while(liveQuery.LoadRow() == LiveQuery::LoadRowResult::GotNext) {
            Issue issue;
            issue.creation_timestamp    = liveQuery.GetInteger();
            issue.issue_id              = liveQuery.GetInteger();
            issue.description           = liveQuery.GetString();
            issue.colaborator_id        = liveQuery.GetInteger();
            issue.state                 = liveQuery.GetInteger();

            issues.push_back(std::move(issue));
        }

        return issues;
    }

    void SaveIssue(std::vector<Issue> const&) {
        DatabaseConnection& database = GetDatabase();
        database.ExecuteQuery("INSERT INTO ISSUES() VALUES()");
    }
}