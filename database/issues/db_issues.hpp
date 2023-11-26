#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Database
{
    struct Issue
    {
        int64_t creation_timestamp;
        int64_t issue_id;
        std::string description;
        int64_t colaborator_id;
        int64_t state;
    };

    auto GetAllIssues() ->std::vector<Issue>;
    void SaveIssue(std::vector<Issue> const&);
    void DeleteIssue(int64_t issue_id);
}

