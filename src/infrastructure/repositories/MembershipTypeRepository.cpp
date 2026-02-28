#include "MembershipTypeRepository.h"
#include <iostream>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

MembershipTypeRepository::MembershipTypeRepository(sqlite3 *connection) : db(connection) {}
MembershipTypeRepository::~MembershipTypeRepository() {}

/* *************************************************************************
                    ---------- INSERT MEMBERSHIP TYPE ----------
   *************************************************************************  */

bool MembershipTypeRepository::insertMembershipType(MembershipType &type)
{

    const char *sql =
        "INSERT INTO membership_types "
        "(membership_name, duration_days, price, "
        "max_borrowing_limit, borrowing_duration_days, "
        "fine_per_day, description) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, type.getMembershipName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, type.getDurationDays()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 3, type.getPrice()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, type.getMaxBorrowingLimit()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, type.getBorrowingDurationDays()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 6, type.getFinePerDay()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, type.getDescription().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
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
        type.setMembershipTypeId(static_cast<int>(lastId));
    }
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- UPDATE MEMBERSHIP TYPE ----------
   *************************************************************************  */

bool MembershipTypeRepository::updateMembershipType(const MembershipType &type)
{

    const char *sql =
        "UPDATE membership_types SET "
        "membership_name=?, duration_days=?, price=?, "
        "max_borrowing_limit=?, borrowing_duration_days=?, "
        "fine_per_day=?, description=? "
        "WHERE membership_type_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, type.getMembershipName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, type.getDurationDays()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 3, type.getPrice()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, type.getMaxBorrowingLimit()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, type.getBorrowingDurationDays()) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 6, type.getFinePerDay()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, type.getDescription().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, type.getMembershipTypeId()) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for UPDATE: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute UPDATE: "
             << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- DELETE MEMBERSHIP TYPE ----------
   *************************************************************************  */

bool MembershipTypeRepository::deleteMembershipType(int typeId)
{

    const char *sql =
        "DELETE FROM membership_types WHERE membership_type_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, typeId) != SQLITE_OK)
    {
        cerr << "Failed to bind typeId for DELETE: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute DELETE: "
             << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                ---------- GET MEMBERSHIP TYPES BY ID's ----------
   *************************************************************************  */

std::unique_ptr<MembershipType> MembershipTypeRepository::getById(int typeId)
{

    const char *sql =
        "SELECT membership_type_id, membership_name, duration_days, price, "
        "max_borrowing_limit, borrowing_duration_days, "
        "fine_per_day, description "
        "FROM membership_types WHERE membership_type_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, typeId) != SQLITE_OK)
    {
        cerr << "Failed to bind typeId in SELECT: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<MembershipType> type = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        type = std::make_unique<MembershipType>(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_int(stmt, 5),
            sqlite3_column_double(stmt, 6),
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return type;
}

/* *************************************************************************
                  ---------- GET ALL MEMBERSHIP TYPES ----------
   *************************************************************************  */

std::vector<MembershipType> MembershipTypeRepository::getAllMembershipTypes()
{

    std::vector<MembershipType> types;

    const char *sql =
        "SELECT membership_type_id, membership_name, duration_days, price, "
        "max_borrowing_limit, borrowing_duration_days, "
        "fine_per_day, description FROM membership_types;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT ALL: "
             << sqlite3_errmsg(db) << endl;
        return types;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        types.emplace_back(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_double(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_int(stmt, 5),
            sqlite3_column_double(stmt, 6),
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return types;
}

/* *************************************************************************
                  ---------- SAVE ALL MEMBERSHIP TYPES ----------
   *************************************************************************  */

bool MembershipTypeRepository::save(MembershipType &type)
{
    if (type.getMembershipTypeId() == 0)
    {
        return insertMembershipType(type);
    }
    return updateMembershipType(type);
}