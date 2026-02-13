#pragma once
extern "C"
{
#include "sqlite3.h"
}
#include <string>
using namespace std;
class SqliteConnection
{
private:
    sqlite3 *db;
    string pathToDB;

public:
    // Constructor
    SqliteConnection(const std::string &path);

    // Destructor
    ~SqliteConnection();

    // Open connection
    bool connect();

    // Close connection
    void disconnect();

    // Execute INSERT / UPDATE / DELETE queries
    bool executeQuery(const std::string &query);

    // Prepare SELECT statements for later use
    sqlite3_stmt *prepareStatement(const std::string &query);

    // Get exposed pointer to the database
    sqlite3 *getConnection() const;
};