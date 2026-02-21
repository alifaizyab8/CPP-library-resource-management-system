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
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "address TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "balance REAL NOT NULL DEFAULT 0.0,"
        "membership_type_id INTEGER NOT NULL,"
        "registration_date TEXT NOT NULL,"
        "is_active INTEGER NOT NULL DEFAULT 1"
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
    const char *fineTable =
        "CREATE TABLE IF NOT EXISTS fines ("
        "fine_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "transaction_id INTEGER NOT NULL,"
        "user_id INTEGER NOT NULL,"
        "days_overdue INTEGER NOT NULL,"
        "fine_amount REAL NOT NULL,"
        "fine_date TEXT NOT NULL,"
        "is_paid INTEGER NOT NULL DEFAULT 0,"
        "payment_date TEXT"
        ");";
    const char *administratorTable =
        "CREATE TABLE IF NOT EXISTS administrators ("
        "admin_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password TEXT NOT NULL,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "created_date TEXT NOT NULL,"
        "is_active INTEGER NOT NULL DEFAULT 1"
        ");";
    const char *fundRequestsTable =
        "CREATE TABLE IF NOT EXISTS fund_requests ("
        "request_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "requested_amount REAL NOT NULL,"
        "request_date TEXT NOT NULL,"
        "status TEXT NOT NULL,"
        "admin_id INTEGER DEFAULT 0,"
        "approval_date TEXT,"
        "admin_notes TEXT"
        ");";
    const char *membershipTypesTable =
        "CREATE TABLE IF NOT EXISTS membership_types ("
        "membership_type_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "membership_name TEXT NOT NULL UNIQUE,"
        "duration_days INTEGER NOT NULL,"
        "price REAL NOT NULL,"
        "max_borrowing_limit INTEGER NOT NULL DEFAULT 2,"
        "borrowing_duration_days INTEGER NOT NULL DEFAULT 14,"
        "fine_per_day REAL NOT NULL DEFAULT 5.00,"
        "description TEXT"
        ");";

    const char *reservationsTable =
        "CREATE TABLE IF NOT EXISTS reservations ("
        "reservation_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "resource_id INTEGER NOT NULL,"
        "reservation_date TEXT NOT NULL,"
        "expiry_date TEXT NOT NULL,"
        "is_fulfilled INTEGER NOT NULL DEFAULT 0,"
        "is_cancelled INTEGER NOT NULL DEFAULT 0,"
        "status TEXT NOT NULL DEFAULT 'PENDING'"
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
    if (sqlite3_exec(db, administratorTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, fineTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, fundRequestsTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, membershipTypesTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db, reservationsTable, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}