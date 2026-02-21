#include "FineRepository.h"
#include <sqlite3.h>
#include <iostream>

using namespace std;

// Constructor
FineRepository::FineRepository(sqlite3* connection)
    : db(connection)
{
}

// Destructor
FineRepository::~FineRepository() {}


// ------------------- Insert Fine -------------------
bool FineRepository::insertFine(const Fine& fine) {

    const char* sql =
        "INSERT INTO fines "
        "(transaction_id, user_id, days_overdue, fine_amount, "
        "fine_date, is_paid, payment_date) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare INSERT: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, fine.getTransactionId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, fine.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, fine.getDaysOverdue()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 4, fine.getFineAmount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, fine.getFineDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 6, fine.getIsPaid() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, fine.getPaymentDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {

        cerr << "Failed to bind parameters for INSERT: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success) {
        cerr << "Failed to execute INSERT: "
             << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}


// ------------------- Update Fine -------------------
bool FineRepository::updateFine(const Fine& fine) {

    const char* sql =
        "UPDATE fines SET "
        "transaction_id=?, user_id=?, days_overdue=?, "
        "fine_amount=?, fine_date=?, is_paid=?, payment_date=? "
        "WHERE fine_id=?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare UPDATE: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, fine.getTransactionId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, fine.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, fine.getDaysOverdue()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 4, fine.getFineAmount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, fine.getFineDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 6, fine.getIsPaid() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, fine.getPaymentDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, fine.getFineId()) != SQLITE_OK) {

        cerr << "Failed to bind parameters for UPDATE: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success) {
        cerr << "Failed to execute UPDATE: "
             << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}


// ------------------- Delete Fine -------------------
bool FineRepository::deleteFine(int fineId) {

    const char* sql =
        "DELETE FROM fines WHERE fine_id=?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare DELETE: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, fineId) != SQLITE_OK) {
        cerr << "Failed to bind fineId: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success) {
        cerr << "Failed to execute DELETE: "
             << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}


// ------------------- Get By ID -------------------
Fine* FineRepository::getById(int fineId) {

    const char* sql =
        "SELECT fine_id, transaction_id, user_id, days_overdue, "
        "fine_amount, fine_date, is_paid, payment_date "
        "FROM fines WHERE fine_id=?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT: "
             << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, fineId) != SQLITE_OK) {
        cerr << "Failed to bind fineId: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt* stmt, int col) -> string {
        const unsigned char* text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char*>(text) : "";
    };

    Fine* fine = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fine = new Fine(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7)
        );
    }

    sqlite3_finalize(stmt);
    return fine;
}


// ------------------- Get By User ID -------------------
std::vector<Fine> FineRepository::getByUserId(int userId) {

    std::vector<Fine> fines;

    const char* sql =
        "SELECT fine_id, transaction_id, user_id, days_overdue, "
        "fine_amount, fine_date, is_paid, payment_date "
        "FROM fines WHERE user_id=?;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT BY USER: "
             << sqlite3_errmsg(db) << endl;
        return fines;
    }

    sqlite3_bind_int(stmt, 1, userId);

    auto safeText = [](sqlite3_stmt* stmt, int col) -> string {
        const unsigned char* text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char*>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        fines.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7)
        );
    }

    sqlite3_finalize(stmt);
    return fines;
}


// ------------------- Get All -------------------
std::vector<Fine> FineRepository::getAllFines() {

    std::vector<Fine> fines;

    const char* sql =
        "SELECT fine_id, transaction_id, user_id, days_overdue, "
        "fine_amount, fine_date, is_paid, payment_date FROM fines;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare SELECT ALL: "
             << sqlite3_errmsg(db) << endl;
        return fines;
    }

    auto safeText = [](sqlite3_stmt* stmt, int col) -> string {
        const unsigned char* text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char*>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        fines.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_double(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7)
        );
    }

    sqlite3_finalize(stmt);
    return fines;
}


// ------------------- Save -------------------
bool FineRepository::save(const Fine& fine) {
    if (fine.getFineId() == 0) {
        return insertFine(fine);
    }
    return updateFine(fine);
}