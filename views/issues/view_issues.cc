#include <db_issues.hpp>

#include <fmt/core.h>
#include <sstream>

std::string GetIssueComponent(Database::Issue issue)
{
    return fmt::format(R"(
        <tr>
            <td>{issue_id}</td>
            <td>{colaborator_id}</td>
            <td>{issue_name}</td>
            <td> <button type="button">Delete issue {issue_id}</button> </td>
        </tr>
        )",
        fmt::arg("issue_id", issue.issue_id),
        fmt::arg("issue_name", issue.description),
        fmt::arg("colaborator_id", issue.colaborator_id),
        fmt::arg("state", issue.state)
    );
}

std::string GetIssueListComponent()
{
    std::vector<Database::Issue> issues = Database::GetAllIssues();
    
    std::stringstream ss;
    for (Database::Issue issue: issues) {
        ss << GetIssueComponent(issue);
    }

    return fmt::format(R"(
<div class="table-wrapper">
    <table class="fl-table">
        <thead>
        <tr>
            <th>{header1}</th>
            <th>{header2}</th>
            <th>{header3}</th>
            <th>{header4}</th>
        </tr>
        </thead>
        <tbody>
            {issueComponents}
        <tbody>
    </table>
</div>
)",
        fmt::arg("issueComponents", ss.str()),
        fmt::arg("header1", "Issue Id"),
        fmt::arg("header2", "Colaborator"),
        fmt::arg("header3", "Description"),
        fmt::arg("header4", "Header 4")
    );
}