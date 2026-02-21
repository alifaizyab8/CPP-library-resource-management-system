#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/entities/User.h"

class UserRepository {
private:
    sqlite3* db;

    bool insertUser(const User& user);
    bool updateUser(const User& user);

public:
    explicit UserRepository(sqlite3* connection);
    ~UserRepository();

    bool save(const User& user);
    bool deleteUser(int userId);

    User* getById(int userId);
    User* getByUsername(const std::string& username);
    std::vector<User> getAllUsers();
};