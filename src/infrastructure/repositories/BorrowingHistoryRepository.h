#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <memory>
#include "../../domain/BorrowingHistory.h"

class BorrowingHistoryRepository
{
private:
    sqlite3 *db;

    // Internal helper methods
    bool insertHistory(BorrowingHistory &history);
    bool updateHistory(const BorrowingHistory &history);

public:
    explicit BorrowingHistoryRepository(sqlite3 *connection);
    ~BorrowingHistoryRepository();

    // CRUD Operations
    bool save(BorrowingHistory &history);
    bool deleteHistory(int historyId);

    // Retrieval Operations
    std::unique_ptr<BorrowingHistory> getById(int historyId);
    std::vector<BorrowingHistory> getAll();
    std::vector<BorrowingHistory> getByUserId(int userId);
};