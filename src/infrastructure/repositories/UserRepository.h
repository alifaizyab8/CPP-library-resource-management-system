#pragma once
#include <vector>
#include "../../domain/entities/User.h"
#include "../../infrastructure/repositories/UserRepository.h"
#include "../database/SqliteConnection.h"
#include <iostream>

using namespace std;

class UserRepository
{
    private:
    sqlite3 *&db; // uses existing database -> Dependency Injection
    std::string getSafeText(sqlite3_stmt* stmt, int col);

public:
    UserRepository(sqlite3 *connection);

    User *findByID(int userId);
    User *findByUsername(const string &username);
    vector<User> getAllUsers();
    bool update(const User &user);
    bool deleteUser(int userId);
};