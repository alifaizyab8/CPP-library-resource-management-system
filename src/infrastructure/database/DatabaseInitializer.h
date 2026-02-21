#pragma once
extern "C"
{
#include "sqlite3.h"
}
#include <string>

class DatabaseInitializer
{
private:
    sqlite3 *db;        // -> points to the database connection.
    std::string dbFile; // -> tores file name.

public:
    // Constructor.
    explicit DatabaseInitializer(const std::string &filename);

    // Destructor.
    ~DatabaseInitializer();

    // Opens database file.
    bool open();

    // Creates tables if they don’t exist.
    bool createTables();

    // Returns database pointer so repositories can use it.
    sqlite3 *getConnection();
};