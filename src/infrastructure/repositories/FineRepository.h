#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <memory>
#include "../../domain/Fine.h"

class FineRepository {
private:
    sqlite3* db;

    bool insertFine(Fine& fine);
    bool updateFine(const Fine& fine);

public:
    explicit FineRepository(sqlite3* connection);
    ~FineRepository();

    bool save(Fine& fine);
    bool deleteFine(int fineId);

    std::unique_ptr<Fine> getById(int fineId);
    std::vector<Fine> getByUserId(int userId);
    std::vector<Fine> getAllFines();
};