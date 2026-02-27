#include "UserRepository.h"
#include <sqlite3.h>
#include <iostream>
#include <memory>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

UserRepository::UserRepository(sqlite3 *connection) : db(connection) {}
UserRepository::~UserRepository() {}

/* *************************************************************************
                        ---------- INSERT USER ----------
   *************************************************************************  */

bool UserRepository::insertUser(User &user)
{
    const char *sql =
        "INSERT INTO users "
        "(username, password, first_name, last_name, email, address, phone, "
        "balance, membership_type_id, registration_date, is_active) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, user.getFirstName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, user.getLastName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, user.getEmail().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, user.getAddress().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, user.getPhone().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 8, user.getBalance()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 9, user.getMembershipTypeId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 10, user.getRegistrationDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 11, user.getIsActive() ? 1 : 0) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for INSERT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute INSERT: " << sqlite3_errmsg(db) << endl;
    else
    {
        // This function takes the Auto Generated ID from the database and assigns it back to the object
        sqlite3_int64 lastId = sqlite3_last_insert_rowid(db);
        user.setUserId(static_cast<int>(lastId));
    }
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                        ---------- UPDATE USER ----------
   *************************************************************************  */

bool UserRepository::updateUser(const User &user)
{

    const char *sql =
        "UPDATE users SET "
        "username=?, password=?, first_name=?, last_name=?, email=?, "
        "address=?, phone=?, balance=?, membership_type_id=?, "
        "registration_date=?, is_active=? "
        "WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, user.getFirstName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, user.getLastName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, user.getEmail().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, user.getAddress().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, user.getPhone().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 8, user.getBalance()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 9, user.getMembershipTypeId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 10, user.getRegistrationDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 11, user.getIsActive() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 12, user.getUserId()) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for UPDATE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute UPDATE: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                        ---------- DELETE USER ----------
   *************************************************************************  */

bool UserRepository::deleteUser(int userId)
{

    const char *sql = "DELETE FROM users WHERE user_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK)
    {
        cerr << "Failed to bind userId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute DELETE: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                        ---------- GET USER BY ID's ----------
   *************************************************************************  */

std::unique_ptr<User> UserRepository::getById(int userId)
{

    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK)
    {
        cerr << "Failed to bind userId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<User> user = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        user = std::make_unique<User>(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7),
            sqlite3_column_double(stmt, 8),
            sqlite3_column_int(stmt, 9),
            safeText(stmt, 10),
            sqlite3_column_int(stmt, 11) == 1);
    }

    sqlite3_finalize(stmt);
    return user;
}

/* *************************************************************************
                    ---------- GET USER BY USERNAME ----------
   *************************************************************************  */

std::unique_ptr<User> UserRepository::getByUsername(const std::string &username)
{

    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users WHERE username=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT BY USERNAME: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        cerr << "Failed to bind username: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<User> user = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        user = std::make_unique<User>(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7),
            sqlite3_column_double(stmt, 8),
            sqlite3_column_int(stmt, 9),
            safeText(stmt, 10),
            sqlite3_column_int(stmt, 11) == 1);
    }

    sqlite3_finalize(stmt);
    return user;
}

/* *************************************************************************
                        ---------- GET ALL USERS ----------
   *************************************************************************  */

std::vector<User> UserRepository::getAllUsers()
{

    std::vector<User> users;

    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT ALL: " << sqlite3_errmsg(db) << endl;
        return users;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        users.emplace_back(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7),
            sqlite3_column_double(stmt, 8),
            sqlite3_column_int(stmt, 9),
            safeText(stmt, 10),
            sqlite3_column_int(stmt, 11) == 1);
    }

    sqlite3_finalize(stmt);
    return users;
}

/* *************************************************************************
                        ---------- SAVE USER ----------
   *************************************************************************  */

bool UserRepository::save(User &user)
{
    if (user.getUserId() == 0)
    {
        return insertUser(user);
    }
    return updateUser(user);
}