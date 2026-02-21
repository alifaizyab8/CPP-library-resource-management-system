#include "DatabaseInitializer.h"
#include <iostream>

// Constructor.
DatabaseInitializer::DatabaseInitializer(const std::string &filename)
    : db(nullptr), dbFile(filename)
{

    int result = sqlite3_open(filename.c_str(), &db);
    if (result != SQLITE_OK)
    {
        std::string errorMsg = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Failed to open database: " + errorMsg);
    }
}

// Destructor.
DatabaseInitializer::~DatabaseInitializer()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

// Open database.
bool DatabaseInitializer::open()
{
    if (sqlite3_open(dbFile.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

// Assign a pointer to the database so that it's accessible elsewhere.
sqlite3 *DatabaseInitializer::getConnection()
{
    return db;
}