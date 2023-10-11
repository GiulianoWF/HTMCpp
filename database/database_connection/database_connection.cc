#include <database_connection.hpp>

using namespace Database;

auto GetDatabase() ->DatabaseConnection&
{
    static DatabaseConnection token_;
    return token_;
}

DatabaseConnection::DatabaseConnection()
{
    auto rc = sqlite3_open("test.db", &this->db);
    if (rc)
    {
        throw "lala";
    }
}

DatabaseConnection::~DatabaseConnection()
{
    sqlite3_close(this->db);
}