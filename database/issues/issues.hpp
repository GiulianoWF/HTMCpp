#pragma once

#include <string>
#include <vector>

namespace Database
{
    struct Issues
    {
        int64_t creation_timestamp;
        std::wstring description;
        int64_t colaborator_id;
        int64_t state;
    };

    auto GetAllIssues() ->std::vector<Issues>;
}

