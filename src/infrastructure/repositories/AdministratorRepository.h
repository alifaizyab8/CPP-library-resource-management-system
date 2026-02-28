#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <memory>
#include "../../domain/Administrator.h"

class AdministratorRepository
{
private:
    sqlite3 *db;

    bool insertAdministrator(Administrator &admin);
    bool updateAdministrator(const Administrator &admin);

public:
    explicit AdministratorRepository(sqlite3 *connection);
    ~AdministratorRepository();

    bool save(Administrator &admin);
    bool deleteAdministrator(int adminId);

    std::unique_ptr<Administrator> getById(int adminId);
    std::vector<Administrator> getAllAdministrators();
};