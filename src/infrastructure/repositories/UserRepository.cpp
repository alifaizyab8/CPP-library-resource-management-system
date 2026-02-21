#include "UserRepository.h"
#include <sqlite3.h>
#include <iostream>

using namespace std;

UserRepository::UserRepository(sqlite3 *connection) : db(connection) {}

// ------------------- Private Helper -------------------
std::string UserRepository::getSafeText(sqlite3_stmt* stmt, int col)
{
    const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
    return text ? text : "";
}

// ------------------- Delete User -------------------
bool UserRepository::deleteUser(int userId)
{
    const char *sql = "DELETE FROM users WHERE user_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK) {
        cerr << "Failed to bind userId in DELETE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        cerr << "Failed to execute DELETE: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

// ------------------- Update User -------------------
bool UserRepository::update(const User &user)
{
    const char *sql =
        "UPDATE users SET address=?, phone=?, balance=?, "
        "membership_type_id=?, registration_date=?, is_active=? "
        "WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare UPDATE statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, user.getAddress().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, user.getPhone().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 3, user.getBalance()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, user.getMembershipTypeId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, user.getRegistrationDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 6, user.getIsActive()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 7, user.getUserId()) != SQLITE_OK) {
        cerr << "Failed to bind parameters in UPDATE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        cerr << "Failed to execute UPDATE: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

// ------------------- Find User by ID -------------------
User* UserRepository::findByID(int userId)
{
    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK) {
        cerr << "Failed to bind userId in SELECT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    User *user = nullptr;
    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW)
    {
        user = new User(
            sqlite3_column_int(stmt, 0),
            getSafeText(stmt, 1),
            getSafeText(stmt, 2),
            getSafeText(stmt, 3),
            getSafeText(stmt, 4),
            getSafeText(stmt, 5),
            getSafeText(stmt, 6),
            getSafeText(stmt, 7),
            sqlite3_column_double(stmt, 8),
            sqlite3_column_int(stmt, 9),
            getSafeText(stmt, 10),
            sqlite3_column_int(stmt, 11)
        );
    }
    else if (step != SQLITE_DONE) {
        cerr << "Error executing SELECT by ID: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return user;
}

// ------------------- Find User by Username -------------------
User* UserRepository::findByUsername(const std::string &username)
{
    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users WHERE username=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        cerr << "Failed to bind username in SELECT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    User *user = nullptr;
    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW)
    {
        user = new User(
            sqlite3_column_int(stmt, 0),
            getSafeText(stmt, 1),
            getSafeText(stmt, 2),
            getSafeText(stmt, 3),
            getSafeText(stmt, 4),
            getSafeText(stmt, 5),
            getSafeText(stmt, 6),
            getSafeText(stmt, 7),
            sqlite3_column_double(stmt, 8),
            sqlite3_column_int(stmt, 9),
            getSafeText(stmt, 10),
            sqlite3_column_int(stmt, 11)
        );
    }
    else if (step != SQLITE_DONE) {
        cerr << "Error executing SELECT by username: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return user;
}

// ------------------- Get All Users -------------------
std::vector<User> UserRepository::getAllUsers()
{
    std::vector<User> users;
    const char *sql =
        "SELECT user_id, username, password, first_name, last_name, email, "
        "address, phone, balance, membership_type_id, registration_date, is_active "
        "FROM users;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT ALL statement: " << sqlite3_errmsg(db) << endl;
        return users;
    }

    while (true)
    {
        int step = sqlite3_step(stmt);
        if (step == SQLITE_ROW) {
            users.emplace_back( // -> using this to construct object directly in vectors
                sqlite3_column_int(stmt, 0),
                getSafeText(stmt, 1),
                getSafeText(stmt, 2),
                getSafeText(stmt, 3),
                getSafeText(stmt, 4),
                getSafeText(stmt, 5),
                getSafeText(stmt, 6),
                getSafeText(stmt, 7),
                sqlite3_column_double(stmt, 8),
                sqlite3_column_int(stmt, 9),
                getSafeText(stmt, 10),
                sqlite3_column_int(stmt, 11)
            );
        }
        else if (step == SQLITE_DONE) {
            break;
        }
        else {
            cerr << "Error executing SELECT ALL: " << sqlite3_errmsg(db) << endl;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return users;
}
