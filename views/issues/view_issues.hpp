#pragma once

#include <db_issues.hpp>
#include <string>

auto GetIssueComponent(Database::Issue issue) ->std::string;
auto GetIssueListComponent() ->std::string;
auto GetIssueInputRowComponent() ->std::string;