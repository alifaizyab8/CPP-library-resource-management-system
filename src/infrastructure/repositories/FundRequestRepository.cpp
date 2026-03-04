#include "FundRequestRepository.h"
#include <iostream>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

FundRequestRepository::FundRequestRepository(sqlite3 *connection) : db(connection) {}
FundRequestRepository::~FundRequestRepository() {}

/* *************************************************************************
                    ---------- INSERT FUND REQUESTS ----------
   *************************************************************************  */

bool FundRequestRepository::insertFundRequest(FundRequest &request)
{

    const char *sql =
        "INSERT INTO fund_requests "
        "(user_id, requested_amount, request_date, status, "
        "admin_id, approval_date, admin_notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, request.getUserId()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 2, request.getRequestedAmount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, request.getRequestDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, request.getStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, request.getAdminId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, request.getApprovalDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, request.getAdminNotes().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for INSERT: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success)
    {
        cerr << "Failed to execute INSERT: "
             << sqlite3_errmsg(db) << endl;
    }
    else
    {
        sqlite3_int64 lastId = sqlite3_last_insert_rowid(db);
        request.setRequestId(static_cast<int>(lastId));
    }
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- UPDATE FUND REQUESTS ----------
   *************************************************************************  */

bool FundRequestRepository::updateFundRequest(const FundRequest &request)
{

    const char *sql =
        "UPDATE fund_requests SET "
        "user_id=?, requested_amount=?, request_date=?, status=?, "
        "admin_id=?, approval_date=?, admin_notes=? "
        "WHERE request_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, request.getUserId()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 2, request.getRequestedAmount()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, request.getRequestDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, request.getStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, request.getAdminId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, request.getApprovalDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, request.getAdminNotes().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, request.getRequestId()) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for UPDATE: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success)
    {
        cerr << "Failed to execute UPDATE: "
             << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- DELETE FUND REQUESTS ----------
   *************************************************************************  */

bool FundRequestRepository::deleteFundRequest(int requestId)
{

    const char *sql =
        "DELETE FROM fund_requests WHERE request_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, requestId) != SQLITE_OK)
    {
        cerr << "Failed to bind requestId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success)
    {
        cerr << "Failed to execute DELETE: "
             << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                ---------- GET FUND REQUESTS BY ID's ----------
   *************************************************************************  */

std::unique_ptr<FundRequest> FundRequestRepository::getById(int requestId)
{

    const char *sql =
        "SELECT request_id, user_id, requested_amount, request_date, "
        "status, admin_id, approval_date, admin_notes "
        "FROM fund_requests WHERE request_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT: "
             << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, requestId) != SQLITE_OK)
    {
        cerr << "Failed to bind requestId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<FundRequest> request = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        request = std::make_unique<FundRequest>(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_double(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return request;
}

/* *************************************************************************
              ---------- GET FUND REQUESTS BY USER ID's ----------
   *************************************************************************  */

std::vector<FundRequest> FundRequestRepository::getByUserId(int userId)
{

    std::vector<FundRequest> requests;

    const char *sql =
        "SELECT request_id, user_id, requested_amount, request_date, "
        "status, admin_id, approval_date, admin_notes "
        "FROM fund_requests WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT BY USER: "
             << sqlite3_errmsg(db) << endl;
        return requests;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK)
    {
        cerr << "Failed to bind userId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return requests;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        requests.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_double(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return requests;
}

/* *************************************************************************
                    ---------- GET ALL FUND REQUESTS ----------
   *************************************************************************  */

std::vector<FundRequest> FundRequestRepository::getAllFundRequests()
{

    std::vector<FundRequest> requests;

    const char *sql =
        "SELECT request_id, user_id, requested_amount, request_date, "
        "status, admin_id, approval_date, admin_notes FROM fund_requests;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT ALL: "
             << sqlite3_errmsg(db) << endl;
        return requests;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        requests.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_double(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5),
            safeText(stmt, 6),
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return requests;
}

/* *************************************************************************
                    ---------- SAVE FUND REQUESTS ----------
   *************************************************************************  */

bool FundRequestRepository::save(FundRequest &request)
{
    if (request.getRequestId() == 0)
    {
        return insertFundRequest(request);
    }
    return updateFundRequest(request);
}