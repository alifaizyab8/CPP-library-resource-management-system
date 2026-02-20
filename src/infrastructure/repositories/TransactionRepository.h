#pragma once
#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include "Transaction.h"

class TransactionRepository {
private:
    sqlite3* db;

public:
    explicit TransactionRepository(sqlite3* connection);
    TransactionRepository::~TransactionRepository();
    // Transaction management
    bool save(const Transaction& transaction);
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // CRUD Operations
    bool insertTransaction(const Transaction& transaction);
    bool updateTransaction(const Transaction& transaction);
    bool deleteTransaction(int transactionId);

    Transaction* getById(int transactionId);
    std::vector<Transaction> getByUserId(int userId);
    std::vector<Transaction> getAllTransactions();

};
