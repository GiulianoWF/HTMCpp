#include <db_issues.hpp>
#include <db_database_connection.hpp>
#include <fmt/core.h>

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

    void SaveIssue(std::vector<Issue> const& issues) {
        DatabaseConnection& database = GetDatabase();
        for(Issue const& issue : issues) {
            database.ExecuteQuery(fmt::format(R"(
                INSERT INTO ISSUES(creation_timestamp, issue_id, description, colaborator_id, state) VALUES(
                {creation_timestamp},
                {issue_id},
                {description},
                {colaborator_id},
                {state}))",

            fmt::arg("creation_timestamp", "2023-01-01"),
            fmt::arg("issue_id", issue.issue_id),
            fmt::arg("description", issue.description),
            fmt::arg("colaborator_id", issue.colaborator_id),
            fmt::arg("state", issue.state)));
        }
    }


    void DeleteIssue(int64_t issue_id) {
        DatabaseConnection& database = GetDatabase();
        database.ExecuteDeleteQuery(fmt::format("DELETE FROM ISSUES WHERE issue_id = {};", issue_id));
    }
}