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

bool DatabaseInitializer::createTables()
{
    const char *userTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT,"
        "password TEXT,"
        "first_name TEXT,"
        "last_name TEXT,"
        "email TEXT,"
        "address TEXT,"
        "phone TEXT,"
        "balance REAL,"
        "membership_type_id INTEGER,"
        "registration_date TEXT,"
        "is_active INTEGER"
        ");";

    const char *transactionTable =
        "CREATE TABLE IF NOT EXISTS transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER,"
        "resource_id INTEGER,"
        "issue_date TEXT,"
        "due_date TEXT,"
        "return_date TEXT,"
        "fine_amount REAL,"
        "is_returned INTEGER,"
        "is_overdue INTEGER,"
        "renewal_count INTEGER,"
        "transaction_status TEXT"
        ");";

    char *errMsg = nullptr;

    if (sqlite3_exec(db, userTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, transactionTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}