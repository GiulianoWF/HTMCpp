#include <issues.hpp>
#include <database_connection.hpp>

namespace Database
{
    auto GetAllIssues() ->std::vector<Issues>
    {
        Issues issue{0, "First issue", 0, 0};

        return {issue};
    }
}