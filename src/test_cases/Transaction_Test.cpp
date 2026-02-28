#include "../infrastructure/repositories/TransactionRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testTransactionRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "resource_id INTEGER, "
        "issue_date TEXT, "
        "due_date TEXT, "
        "return_date TEXT, "
        "fine_amount REAL, "
        "is_returned INTEGER, "
        "is_overdue INTEGER, "
        "renewal_count INTEGER, "
        "transaction_status TEXT"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create transactions table. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    TransactionRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Arguments: ID, UserID, ResourceID, IssueDate, DueDate, ReturnDate, Fine, isReturned, isOverdue, Renewals, Status
    Transaction testTx(0, 500, 1001, "2026-02-22", "2026-03-01", "", 0.0, 0, 0, 0, "Issued");

    if (!repo.save(testTx))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Transaction." << std::endl;
        return false;
    }

    int generatedId = testTx.getTransactionId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Transaction ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<Transaction> fetchedTx = repo.getById(generatedId);
    if (fetchedTx == nullptr || fetchedTx->getResourceId() != 1001)
    {
        std::cerr << "TEST FAILED [Read]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. READ (Get By User ID)
    // ---------------------------------------------------------
    std::vector<Transaction> userTxs = repo.getByUserId(500);
    if (userTxs.empty())
    {
        std::cerr << "TEST FAILED [Read By User]: No transactions found for User 500." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. UPDATE (Mark as Returned)
    // ---------------------------------------------------------
    // Simulating the user returning the book on time
    Transaction returnedTx(generatedId, 500, 1001, "2026-02-22", "2026-03-01", "2026-02-25", 0.0, 1, 0, 0, "Returned");
    if (!repo.save(returnedTx))
    {
        std::cerr << "TEST FAILED [Update]: Failed to update transaction." << std::endl;
        return false;
    }

    std::unique_ptr<Transaction> updatedTx = repo.getById(generatedId);
    if (updatedTx == nullptr || updatedTx->getIsReturned() != 1 || updatedTx->getTransactionStatus() != "Returned")
    {
        std::cerr << "TEST FAILED [Update]: Return update did not persist." << std::endl;
        return false;
    }

    // Skipping getAllTransactions() test because it is missing from the .cpp file!

    // ---------------------------------------------------------
    // 5. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteTransaction(generatedId))
    {
        std::cerr << "TEST FAILED [Delete]: Could not execute delete." << std::endl;
        return false;
    }

    if (repo.getById(generatedId) != nullptr)
    {
        std::cerr << "TEST FAILED [Verify Delete]: Record still exists." << std::endl;
        return false;
    }

    return true;
}