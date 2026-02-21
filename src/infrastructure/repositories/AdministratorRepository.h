#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/entities/Administrator.h"

class AdministratorRepository {
private:
    sqlite3* db;

    bool insertAdministrator(const Administrator& admin);
    bool updateAdministrator(const Administrator& admin);

public:
    explicit AdministratorRepository(sqlite3* connection);
    ~AdministratorRepository();

    bool save(const Administrator& admin);
    bool deleteAdministrator(int adminId);

    Administrator* getById(int adminId);
    std::vector<Administrator> getAllAdministrators();
};