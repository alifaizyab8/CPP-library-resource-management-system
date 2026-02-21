#include "TransactionRepository.h"
#include <sqlite3.h>
#include <iostream>
#include <memory>

using namespace std;

// Constructor -> this does not open the database, it only stores the pointer.
TransactionRepository::TransactionRepository(sqlite3 *connection)
    : db(connection)
{
}

// Destructor
TransactionRepository::~TransactionRepository() {}

// ------------------- Transaction Control -------------------

// ------------------- Begin Transaction -------------------
//                starts a database transaction.
bool TransactionRepository::beginTransaction()
{
    char *errMsg = nullptr; // -> to store error message in pointers
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to begin transaction: " << (errMsg ? errMsg : "Unknown error") << endl; // cerr means character error stream.
        if (errMsg)
            sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ------------------- Commit Transaction -------------------
//                 This permanently saves changes.
bool TransactionRepository::commitTransaction()
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to commit transaction: " << (errMsg ? errMsg : "Unknown error") << endl;
        if (errMsg)
            sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ------------------- Roll Back Transaction -------------------
//                  This undoes changes since BEGIN.
bool TransactionRepository::rollbackTransaction()
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to rollback transaction: " << (errMsg ? errMsg : "Unknown error") << endl;
        if (errMsg)
            sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ------------------- Insert Transaction -------------------
bool TransactionRepository::insertTransaction(Transaction &transaction)
{
    const char *sql =
        "INSERT INTO transactions (user_id, resource_id, issue_date, due_date, return_date, "
        "fine_amount, is_returned, is_overdue, renewal_count, transaction_status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, transaction.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, transaction.getResourceId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, transaction.getIssueDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, transaction.getDueDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, transaction.getReturnDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 6, transaction.getFineAmount()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 7, transaction.getIsReturned()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, transaction.getIsOverdue()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 9, transaction.getRenewalCount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 10, transaction.getTransactionStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        cerr << "Failed to bind parameters for INSERT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
    {
        cerr << "Failed to execute INSERT: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        sqlite3_int64 lastId = sqlite3_last_insert_rowid(db);
        transaction.setTransactionId(static_cast<int>(lastId));
    }
    sqlite3_finalize(stmt);
    return success;
}

// ------------------- Update Transaction -------------------
bool TransactionRepository::updateTransaction(const Transaction &transaction)
{
    const char *sql =
        "UPDATE transactions SET user_id=?, resource_id=?, issue_date=?, due_date=?, return_date=?, "
        "fine_amount=?, is_returned=?, is_overdue=?, renewal_count=?, transaction_status=? "
        "WHERE transaction_id=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, transaction.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, transaction.getResourceId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, transaction.getIssueDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, transaction.getDueDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, transaction.getReturnDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 6, transaction.getFineAmount()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 7, transaction.getIsReturned()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, transaction.getIsOverdue()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 9, transaction.getRenewalCount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 10, transaction.getTransactionStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 11, transaction.getTransactionId()) != SQLITE_OK)
    {
        cerr << "Failed to bind parameters for UPDATE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
    {
        cerr << "Failed to execute UPDATE: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

// ------------------- Delete Transaction -------------------
bool TransactionRepository::deleteTransaction(int transactionId)
{
    const char *sql = "DELETE FROM transactions WHERE transaction_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, transactionId) != SQLITE_OK)
    {
        cerr << "Failed to bind transactionId for DELETE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute DELETE: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

// ------------------- Find by ID -------------------
std::unique_ptr<Transaction> TransactionRepository::getById(int transactionId)
{
    const char *sql =
        "SELECT transaction_id, user_id, resource_id, issue_date, due_date, return_date, "
        "fine_amount, is_returned, is_overdue, renewal_count, transaction_status "
        "FROM transactions WHERE transaction_id=?;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, transactionId) != SQLITE_OK)
    {
        cerr << "Failed to bind transactionId in SELECT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> std::string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<Transaction> transaction = nullptr;
    int step = sqlite3_step(stmt);

    if (step == SQLITE_ROW)
    {
        transaction = std::make_unique<Transaction>(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_double(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            safeText(stmt, 10));
    }

    sqlite3_finalize(stmt);
    return transaction;
}

// ------------------- Get Transactions by User ID -------------------
std::vector<Transaction> TransactionRepository::getByUserId(int userId)
{

    std::vector<Transaction> transactions;

    const char *sql =
        "SELECT transaction_id, user_id, resource_id, issue_date, due_date, return_date, "
        "fine_amount, is_returned, is_overdue, renewal_count, transaction_status "
        "FROM transactions WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT BY USER statement: "
             << sqlite3_errmsg(db) << endl;
        return transactions;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK)
    {
        cerr << "Failed to bind userId in SELECT: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return transactions;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> std::string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        transactions.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_double(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            safeText(stmt, 10));
    }

    sqlite3_finalize(stmt);
    return transactions;
}

// ------------------- Save Transaction -------------------
bool TransactionRepository::save(Transaction &transaction)
{

    if (transaction.getTransactionId() == 0)
    {
        return insertTransaction(transaction);
    }
    else
    {
        return updateTransaction(transaction);
    }
}