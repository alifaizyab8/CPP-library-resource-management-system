#include "AdministratorRepository.h"
#include <iostream>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

AdministratorRepository::AdministratorRepository(sqlite3 *connection) : db(connection) {}
AdministratorRepository::~AdministratorRepository() {}

/* *************************************************************************
                 ---------- INSERT ADMINISTRATOR ----------
   *************************************************************************  */

bool AdministratorRepository::insertAdministrator(Administrator &admin)
{

    const char *sql =
        "INSERT INTO administrators "
        "(username, password, first_name, last_name, email, created_date, is_active) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT statement: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, admin.getUsername().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, admin.getPassword().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, admin.getFirstName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, admin.getLastName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, admin.getEmail().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, admin.getCreatedDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 7, admin.getIsActive() ? 1 : 0) != SQLITE_OK)
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
    // Same Feature of Auto ID Assignment from Database
    else
    {
        sqlite3_int64 lastId = sqlite3_last_insert_rowid(db);
        admin.setAdminId(static_cast<int>(lastId));
    }

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                 ---------- UPDATE ADMINISTRATOR ----------
   *************************************************************************  */

bool AdministratorRepository::updateAdministrator(const Administrator &admin)
{

    const char *sql =
        "UPDATE administrators SET "
        "username=?, password=?, first_name=?, last_name=?, "
        "email=?, created_date=?, is_active=? "
        "WHERE admin_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE statement: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, admin.getUsername().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, admin.getPassword().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, admin.getFirstName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, admin.getLastName().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, admin.getEmail().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 6, admin.getCreatedDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 7, admin.getIsActive() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, admin.getAdminId()) != SQLITE_OK)
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
                 ---------- DELETE ADMINISTRATOR ----------
   *************************************************************************  */

bool AdministratorRepository::deleteAdministrator(int adminId)
{

    const char *sql =
        "DELETE FROM administrators WHERE admin_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE statement: "
             << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, adminId) != SQLITE_OK)
    {
        cerr << "Failed to bind adminId for DELETE: "
             << sqlite3_errmsg(db) << endl;
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
                 ---------- FIND ADMINISTRATORS BY ID's ----------
   *************************************************************************  */

std::unique_ptr<Administrator> AdministratorRepository::getById(int adminId)
{

    const char *sql =
        "SELECT admin_id, username, password, first_name, last_name, "
        "email, created_date, is_active "
        "FROM administrators WHERE admin_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT statement: "
             << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, adminId) != SQLITE_OK)
    {
        cerr << "Failed to bind adminId in SELECT: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<Administrator> admin = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        admin = std::make_unique<Administrator>(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            safeText(stmt, 6),
            sqlite3_column_int(stmt, 7) == 1);
    }

    sqlite3_finalize(stmt);
    return admin;
}

/* *************************************************************************
                 ---------- GET ALL ADMINISTRATORS ----------
   *************************************************************************  */

std::vector<Administrator> AdministratorRepository::getAllAdministrators()
{

    std::vector<Administrator> admins;

    const char *sql =
        "SELECT admin_id, username, password, first_name, last_name, "
        "email, created_date, is_active FROM administrators;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT ALL statement: "
             << sqlite3_errmsg(db) << endl;
        return admins;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        admins.emplace_back(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            safeText(stmt, 5),
            safeText(stmt, 6),
            sqlite3_column_int(stmt, 7) == 1);
    }

    sqlite3_finalize(stmt);
    return admins;
}

/* *************************************************************************
                 ---------- SAVE ADMINISTRATOR ----------
   *************************************************************************  */

bool AdministratorRepository::save(Administrator &admin)
{
    if (admin.getAdminId() == 0)
    {
        return insertAdministrator(admin);
    }
    return updateAdministrator(admin);
}