#pragma once

#include <issues.hpp>
#include <fmt/core.h>

std::string GetIssueComponent(Database::Issues issue)
{
    return fmt::format(R"(
            <li class="list-group-itemd-flex align-items-center border-0 mb-2 rounded"
                style="background-color: #f4f6f7;">
                <input class="form-check-input me-2"
                       type="checkbox"
                       value=""
                       aria-label="..."
                       checked
                />
                <s>
                    {issue_name}
                </s>
            </li>
        )",
        fmt::arg("issue_name", issue.description),
        fmt::arg("number", 42)
    );
}

std::string GetIssueListComponent()
{
    std::vector<Database::Issues> issues = Database::GetAllIssues();

    return fmt::format(R"(
        <div
        class="tab-pane fade show active"
        id="ex1-tabs-1"
        role="tabpanel"
        aria-labelledby="ex1-tab-1">
            <ul class="list-group mb-0">
                {component}
            </ul>
        </div>)",
        fmt::arg("component", GetIssueComponent(issues[0])),
        fmt::arg("number", 42)
    );
}