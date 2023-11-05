#include <db_database_connection.hpp>

namespace Database {

auto GetDatabase() ->DatabaseConnection& {
    static DatabaseConnection token_;
    return token_;
}

DatabaseConnection::DatabaseConnection() {
    auto rc = sqlite3_open("test.db", &this->db);
    if (rc)
    {
        throw "lala";
    }
}

DatabaseConnection::~DatabaseConnection() {
    sqlite3_close(this->db);
}

auto DatabaseConnection::ExecuteQuery(std::string const& s) ->LiveQuery {
    char *zErrMsg = 0;
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, s.c_str(), -1, &stmt, NULL);
    if( rc != SQLITE_OK )
    {
        std::cout<<"SQL error: "<<sqlite3_errmsg(db)<<"\n";
        sqlite3_free(zErrMsg);
    }

    return LiveQuery(this->db, stmt, zErrMsg);
};

LiveQuery::LiveQuery(sqlite3 *db, sqlite3_stmt *stmt, char *zErrMsg) {
    this->stmt = stmt;
    this->zErrMsg = zErrMsg;
}

LiveQuery::~LiveQuery() {
    sqlite3_finalize(this->stmt);
}

auto LiveQuery::LoadRow() ->LoadRowResult {
    int result;
    result = sqlite3_step(stmt);

    if (result == SQLITE_DONE)
    {
        return LoadRowResult::Finished;
    }

    if (result != SQLITE_ROW) {
        std::cout<<"SQL error: "<<sqlite3_errmsg(db)<<"\n";
        sqlite3_free(zErrMsg);

        return LoadRowResult::Error;
    }
    
    this->columnIndex = -1;
    
    return LoadRowResult::GotNext;
}

auto LiveQuery::GetInteger() ->int64_t {
    this->columnIndex = this->columnIndex + 1;

    return sqlite3_column_int64(this->stmt, this->columnIndex);
}

auto LiveQuery::GetDouble() ->double {
    this->columnIndex = this->columnIndex + 1;

    return sqlite3_column_double(this->stmt, this->columnIndex);
}

auto LiveQuery::GetString() ->std::string {
    this->columnIndex = this->columnIndex + 1;

    return std::string(reinterpret_cast<const char*>(
        sqlite3_column_text(this->stmt, this->columnIndex)
    ));
}

}