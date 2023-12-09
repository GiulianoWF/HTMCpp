#include <view_landpage.hpp>
#include <view_issues.hpp>
#include <fmt/core.h>

auto GetLandpage() ->std::string {
    return fmt::format(R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Issue tracker</title>
    <link type="text/css" rel="stylesheet" href="style.scss" />
    <link type="text/css" rel="stylesheet" href="issues_table.scss" />
    <!-- Include HTMX library -->
    <script src="https://unpkg.com/htmx.org@1.6.1"></script>
</head>
<body>
{landpage_body}
</body>
</html>
)",
        fmt::arg("landpage_body", GetLandpageBody())
    );
}

auto GetLandpageBody() ->std::string {
        return fmt::format(R"(
    <section class="vh-100 gradient-custom">
    <div class="container py-5 h-100">
        <div class="row d-flex justify-content-center align-items-center h-100">
        <div class="col col-xl-10">
        {issues}
        </div>
    </div>
    </section>
)",
        fmt::arg("issues", GetIssueListComponent())
    );
}