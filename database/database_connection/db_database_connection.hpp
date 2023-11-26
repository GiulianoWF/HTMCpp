#pragma once

#include <functional>
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <optional>
#include <iostream>

#include <pqxx/pqxx>

using namespace std::placeholders;

namespace Database {
    template <typename T>
    concept IsOptional = requires (T opt) {
        { std::optional<typename T::value_type>{opt} };
    };

    template <typename T>
    concept IsNotOptional = !IsOptional<T>;

    class LiveQuery {
        private:
            int columnIndex = 0;
            char* zErrMsg = nullptr;
            sqlite3_stmt* stmt = nullptr;
            sqlite3* db = nullptr;

        public:
            ~LiveQuery();
            LiveQuery(sqlite3 *db, sqlite3_stmt *stmt, char *zErrMsg);

            enum class LoadRowResult: int64_t {
                GotNext = 0,
                Finished = 1,
                Error = 2,
            };
            auto LoadRow() ->LoadRowResult;
            
            auto GetInteger() ->int64_t;
            auto GetDouble() ->double;
            auto GetString() ->std::string;
    };

    class DatabaseConnection {
        sqlite3 *db;
        int returnCode = SQLITE_OK;
        int lastQueryArgCount;
        int lastQueryArgCountLastRead;
        char ** lastQueryArgVector;
        char ** lastQueryColNameVector;

        public:
            DatabaseConnection();
            ~DatabaseConnection();

            auto ExecuteQuery(std::string const& s) -> LiveQuery;
            void ExecuteDeleteQuery(std::string const& s);
    };

    auto GetDatabase() ->DatabaseConnection&;
}