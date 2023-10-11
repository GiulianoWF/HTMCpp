#pragma once

#include <sqlite3.h>

namespace Database {

    class DatabaseConnection {
        int i;
        sqlite3 *db;

        public:
            DatabaseConnection();
            ~DatabaseConnection();
    };

    auto GetDatabase() ->DatabaseConnection&;
}