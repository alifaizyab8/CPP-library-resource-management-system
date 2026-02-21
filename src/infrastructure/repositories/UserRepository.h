#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <memory>
#include "../../domain/User.h"

class UserRepository
{
private:
    sqlite3 *db;

    bool insertUser(User &user);
    bool updateUser(const User &user);

public:
    explicit UserRepository(sqlite3 *connection);
    ~UserRepository();

    bool save(User &user);
    bool deleteUser(int userId);

    std::unique_ptr<User> getById(int userId);
    std::unique_ptr<User> getByUsername(const std::string &username);
    std::vector<User> getAllUsers();
};