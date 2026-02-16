#include "SqliteConnection.h"
#include <iostream>

#include "SQLiteConnection.h"
#include <iostream>

using namespace std;

// Constructor
SqliteConnection::SqliteConnection(const string &path)
    : db(nullptr), pathToDB(path)
{
}

// Destructor
SqliteConnection::~SqliteConnection()
{
    disconnect();
}

// Connect to database
bool SqliteConnection::connect()
{
    int rc = sqlite3_open(pathToDB.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

// Disconnect from database
void SqliteConnection::disconnect()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

// Execute query INSERT, UPDATE, DELETE
bool SqliteConnection::executeQuery(const string &query)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Prepare statement SELECT
sqlite3_stmt *SqliteConnection::prepareStatement(const string &query)
{
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    return stmt;
}

// Get raw connection pointer
sqlite3 *SqliteConnection::getConnection() const
{
    return db;
}