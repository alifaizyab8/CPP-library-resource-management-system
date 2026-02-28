#include "BorrowingHistoryRepository.h"
#include <iostream>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

BorrowingHistoryRepository::BorrowingHistoryRepository(sqlite3 *connection) : db(connection) {}
BorrowingHistoryRepository::~BorrowingHistoryRepository() {}

/* *************************************************************************
                ---------- INSERT BORROWING HISTORY ----------
   *************************************************************************  */

bool BorrowingHistoryRepository::insertHistory(BorrowingHistory &history)
{
    const char *sql = "INSERT INTO borrowing_history (user_id, resource_id, issue_date, due_date, return_date, fine_amount) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Prepare Insert Failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    // Binding parameters - Foreign keys (user_id, resource_id) must exist in their respective tables
    sqlite3_bind_int(stmt, 1, history.getUserId());
    sqlite3_bind_int(stmt, 2, history.getResourceId());
    sqlite3_bind_text(stmt, 3, history.getIssueDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, history.getDueDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, history.getReturnDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, history.getFineAmount());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (success)
    {
        history.setId(static_cast<int>(sqlite3_last_insert_rowid(db)));
    }
    else
    {
        cerr << "Execution Failed! Check Foreign Key!" << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                ---------- UPDATE BORROWING HISTORY ----------
   *************************************************************************  */

bool BorrowingHistoryRepository::updateHistory(const BorrowingHistory &history)
{
    const char *sql = "UPDATE borrowing_history SET user_id=?, resource_id=?, issue_date=?, due_date=?, "
                      "return_date=?, fine_amount=? WHERE history_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_int(stmt, 1, history.getUserId());
    sqlite3_bind_int(stmt, 2, history.getResourceId());
    sqlite3_bind_text(stmt, 3, history.getIssueDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, history.getDueDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, history.getReturnDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, history.getFineAmount());
    sqlite3_bind_int(stmt, 7, history.getId());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                ---------- SAVE BORROWING HISTORY ----------
   *************************************************************************  */

bool BorrowingHistoryRepository::save(BorrowingHistory &history)
{
    if (history.getId() == 0)
    {
        return insertHistory(history);
    }
    return updateHistory(history);
}

/* *************************************************************************
                ---------- DELETE BORROWING HISTORY ----------
   *************************************************************************  */

bool BorrowingHistoryRepository::deleteHistory(int historyId)
{
    const char *sql = "DELETE FROM borrowing_history WHERE history_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, historyId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                ---------- GET BORROWING HISTORY BY ID's ----------
   *************************************************************************  */

unique_ptr<BorrowingHistory> BorrowingHistoryRepository::getById(int historyId)
{
    const char *sql = "SELECT history_id, user_id, resource_id, issue_date, due_date, return_date, fine_amount "
                      "FROM borrowing_history WHERE history_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return nullptr;
    sqlite3_bind_int(stmt, 1, historyId);

    unique_ptr<BorrowingHistory> history = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        // Explicitly reading text columns without using lambda helpers
        const unsigned char *issueText = sqlite3_column_text(stmt, 3);
        const unsigned char *dueText = sqlite3_column_text(stmt, 4);
        const unsigned char *returnText = sqlite3_column_text(stmt, 5);

        history = make_unique<BorrowingHistory>(
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            issueText ? reinterpret_cast<const char *>(issueText) : "",
            dueText ? reinterpret_cast<const char *>(dueText) : "",
            returnText ? reinterpret_cast<const char *>(returnText) : "",
            sqlite3_column_double(stmt, 6));
        history->setId(sqlite3_column_int(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return history;
}

/* *************************************************************************
                ---------- GET ALL BORROWING HISTORY ----------
   *************************************************************************  */

vector<BorrowingHistory> BorrowingHistoryRepository::getAll()
{
    vector<BorrowingHistory> results;
    const char *sql = "SELECT history_id, user_id, resource_id, issue_date, due_date, return_date, fine_amount FROM borrowing_history;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return results;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *issueText = sqlite3_column_text(stmt, 3);
        const unsigned char *dueText = sqlite3_column_text(stmt, 4);
        const unsigned char *returnText = sqlite3_column_text(stmt, 5);

        BorrowingHistory bh(
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            issueText ? reinterpret_cast<const char *>(issueText) : "",
            dueText ? reinterpret_cast<const char *>(dueText) : "",
            returnText ? reinterpret_cast<const char *>(returnText) : "",
            sqlite3_column_double(stmt, 6));
        bh.setId(sqlite3_column_int(stmt, 0));
        results.push_back(bh);
    }

    sqlite3_finalize(stmt);
    return results;
}