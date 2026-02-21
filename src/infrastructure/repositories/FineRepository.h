#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/entities/Fine.h"

class FineRepository {
private:
    sqlite3* db;

    bool insertFine(const Fine& fine);
    bool updateFine(const Fine& fine);

public:
    explicit FineRepository(sqlite3* connection);
    ~FineRepository();

    bool save(const Fine& fine);
    bool deleteFine(int fineId);

    Fine* getById(int fineId);
    std::vector<Fine> getByUserId(int userId);
    std::vector<Fine> getAllFines();
};