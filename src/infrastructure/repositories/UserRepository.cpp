#include <iostream>
#include "../../infrastructure/database/SqliteConnection.h"
#include "../..infrastructure/repositories/UserRepository.h"
#include "../../domain/entities/User.h"
#include <vector>
#include <string>

using namespace std;

UserRepository ::UserRepository(sqlite3 *connection) : db(connection) {}
// When created, they are attached to the database

void UserRepository ::addUser(string name, string email)
{
    const char *sql = "INSERT INTO users(name,email) VALUES (?,?);";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); // Prepare -> Complies SQL
    // Q: Why is this important?
    /* A: They prevent SQL injection (someone tries to run their own query),
       runs faster, and reuses the query for each repo*/

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC); // Binds -> Insertion of values are safe
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);

    sqlite3_step(stmt); // Step -> Runs the query

    sqlite3_finalize(stmt); // Finalize -> Frees/Cleans Memory
}
void UserRepository ::deleteUser(int id)
{
    const char *sql = "DELETE FROM users WHERE id=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void UserRepository ::updateUser(int id, string name, string email)
{
    const char *sql = "UPDATE users SET name=?, email =? WHERE id=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int UserRepository ::getUserByID(int id)
{
    const char *sql = "SELECT id, name, email FROM users WHERE id=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << sqlite3_column_int(stmt, 0) << " " << sqlite3_column_text(stmt, 1) << " " << sqlite3_column_text(stmt, 2) << endl;
    }
    else
    {
        cout << "User Not Found!" << endl;
    }
    sqlite3_finalize(stmt);
}

string UserRepository ::getAllUsers()
{
    const char *sql = "SELECT id, name, email FROM users;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << sqlite3_column_int(stmt, 0) << " " << sqlite3_column_text(stmt, 1) << " " << sqlite3_column_text(stmt, 2) << endl;
    }

    sqlite3_finalize(stmt);
}

vector<User> UserRepository ::searchUsers(const string &keyword)
{
    vector<User> results;
    const char *sql = "SELECT id, name, email FROM users WHERE name LIKE ? OR email LIKE ?;";
    // LIKE -> allows partial matches in SQL.. It will match anything containing the word "%" + keyword + "%"
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        results.emplace_back(sqlite3_column_int(stmt, 0), (char *)sqlite3_column_text(stmt, 1), (char *)sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
    return results;
}

void UserRepository ::findByName(string name)
{
    const char *sql = "SELECT id, name, email FROM users WHERE name=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << sqlite3_column_int(stmt, 0) << " " << sqlite3_column_text(stmt, 1) << " " << sqlite3_column_text(stmt, 2) << endl;
    }
    sqlite3_finalize(stmt);
}

void UserRepository ::findByEmail(string email)
{
    const char *sql = "SELECT id, name, email FROM users WHERE email=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << "User Exists!" << endl;
    }
    else
    {
        cout << "User Not Found!" << endl;
    }
    sqlite3_finalize(stmt);
}

void UserRepository ::emailExists(string email)
{
    const char *sql = "SELECT 1 FROM users WHERE email=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << "Email Exists!" << endl;
    }
    else
    {
        cout << "Email Available!" << endl;
    }
    sqlite3_finalize(stmt);
}

void UserRepository ::verifyLogin(string email, string password)
{
    const char *sql = "SELECT password FROM users WHERE email=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string stored = (char *)sqlite3_column_text(stmt, 0);
        if (stored == password)
            cout << "Login success!" << endl;
        else
            cout << "Wrong password" << endl;
    }
    else
    {
        cout << "Email not found!" << endl;
    }

    sqlite3_finalize(stmt);
}

void UserRepository ::getPasswordHash(int id)
{
    const char *sql = "SELECT password FROM users WHERE id=?;";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << sqlite3_column_text(stmt, 0) << endl;

        sqlite3_finalize(stmt);
    }
}