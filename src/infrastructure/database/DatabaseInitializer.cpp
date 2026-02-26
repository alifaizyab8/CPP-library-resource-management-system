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
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
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
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    return true;
}

// Assign a pointer to the database so that it's accessible elsewhere.
sqlite3 *DatabaseInitializer::getConnection()
{
    return db;
}

bool DatabaseInitializer::createTables()
{
    // For Foreign Keys to work, the independent tables must be created first
    // Independent Tables: Those Tables which do not have any foreign key in them
    const char *categoriesTable =
        "CREATE TABLE IF NOT EXISTS categories ("
        "category_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "description TEXT"
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

   

    // Now creating Dependent Tables which will use the Primary Keys of the above tables as foreign keys
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
        "is_active INTEGER NOT NULL DEFAULT 1,"
        "FOREIGN KEY(membership_type_id) REFERENCES membership_types(membership_type_id) ON DELETE RESTRICT"
        ");";
    const char *resourcesTable =
        "CREATE TABLE IF NOT EXISTS resources ("
        "resource_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "author TEXT NOT NULL,"
        "publisher TEXT NOT NULL,"
        "publication_year INTEGER NOT NULL,"
        "isbn TEXT NOT NULL,"
        "category_id INTEGER NOT NULL,"
        "resource_type_id INTEGER NOT NULL,"
        "total_copies INTEGER NOT NULL DEFAULT 1,"
        "available_copies INTEGER NOT NULL DEFAULT 1,"
        "description TEXT,"
        "added_date TEXT NOT NULL,"
        "is_active INTEGER NOT NULL DEFAULT 1,"
        "FOREIGN KEY(category_id) REFERENCES categories(category_id) ON DELETE RESTRICT,"
        "FOREIGN KEY(resource_type_id) REFERENCES resource_types(resource_type_id) ON DELETE RESTRICT"
        ");";
    const char *transactionTable =
        "CREATE TABLE IF NOT EXISTS transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER,"
        "resource_id INTEGER,"
        "issue_date TEXT,"
        "due_date TEXT,"
        "return_date TEXT,"
        "fine_amount REAL DEFAULT 0.0,"
        "is_returned INTEGER DEFAULT 0,"
        "is_overdue INTEGER DEFAULT 0,"
        "renewal_count INTEGER DEFAULT 0,"
        "transaction_status TEXT DEFAULT 'ACTIVE',"
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE,"
        "FOREIGN KEY(resource_id) REFERENCES resources(resource_id) ON DELETE CASCADE"
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
        "payment_date TEXT,"
        "FOREIGN KEY(transaction_id) REFERENCES transactions(transaction_id) ON DELETE CASCADE,"
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE"
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
        "admin_notes TEXT,"
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE,"
        "FOREIGN KEY(admin_id) REFERENCES administrators(admin_id) ON DELETE SET NULL"
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
        "status TEXT NOT NULL DEFAULT 'PENDING',"
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE,"
        "FOREIGN KEY(resource_id) REFERENCES resources(resource_id) ON DELETE CASCADE"
        ");";

    const char *borrowingHistoryTable =
        "CREATE TABLE IF NOT EXISTS borrowing_history ("
        "history_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "resource_id INTEGER NOT NULL,"
        "issue_date TEXT NOT NULL,"
        "due_date TEXT NOT NULL,"
        "return_date TEXT,"
        "fine_amount REAL DEFAULT 0.0,"
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE,"
        "FOREIGN KEY(resource_id) REFERENCES resources(resource_id) ON DELETE CASCADE"
        ");";

    // I have removed the multiple if statements and iterated over an array of sql-queries
    // This is because the order of creation of tables matter for the woring of foreign keys

    char *errMsg = nullptr;

    const char *SQLiteTableQueries[] =
        {
            categoriesTable,
            membershipTypesTable,
            administratorTable,
            userTable,
            resourcesTable,
            transactionTable,
            fineTable,
            fundRequestsTable,
            reservationsTable,
            borrowingHistoryTable,

        };
    for (const char *table : SQLiteTableQueries)
    {
        if (sqlite3_exec(db, table, nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::cerr << "Error creating table: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
    }

    return true;
}