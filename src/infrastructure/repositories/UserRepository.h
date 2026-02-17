#pragma once
#include<vector>
#include "../../domain/entities/User.h"
#include "../../infrastructure/repositories/UserRepository.h"
#include"../database/SqliteConnection.h"
#include <iostream>

using namespace std;

class UserRepository
{
    sqlite3 *&db; // uses existing database -> Dependency Injection

public:
    UserRepository(sqlite3 *connection);

    void addUser(string name, string email);
    void deleteUser(int id);
    void updateUser(int id, string name, string email);
    int getUserByID(int id);
    string getAllUsers();
    vector<User> searchUsers(const string &keyword);
    void findByName(string name);
    void findByEmail(string email);
    void verifyLogin(string email, string password);
    void emailExists(string email);
    void getPasswordHash(int id);
};