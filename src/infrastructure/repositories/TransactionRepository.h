#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include "../../domain/Transaction.h"
#include <memory>
class TransactionRepository
{
private:
    sqlite3 *db;

public:
    explicit TransactionRepository(sqlite3 *connection);
    ~TransactionRepository();
    // Transaction management
    bool save(Transaction &transaction);
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // CRUD Operations
    bool insertTransaction(Transaction &transaction);
    bool updateTransaction(const Transaction &transaction);
    bool deleteTransaction(int transactionId);

    std::unique_ptr<Transaction> getById(int transactionId);
    std::vector<Transaction> getByUserId(int userId);
    std::vector<Transaction> getAllTransactions();
};
