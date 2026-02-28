#include "ResourceRepository.h"
#include <sqlite3.h>
#include <iostream>
#include <memory>

using namespace std;

/* *************************************************************************
               ---------- HELPER FOR NULL-SAFE TEXT ----------
   *************************************************************************  */

string safeText(sqlite3_stmt *stmt, int col)
{
    const unsigned char *txt = sqlite3_column_text(stmt, col);
    if (!txt)
        return "";
    return string(reinterpret_cast<const char *>(txt));
}

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

ResourceRepository::ResourceRepository(sqlite3 *connection) : db(connection)
{
    // Enables foreign keys
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
}

ResourceRepository::~ResourceRepository() {}

/* *************************************************************************
                     ---------- INSERT RESOURCES ----------
   *************************************************************************  */

bool ResourceRepository::insertResource(Resource &resource)
{
    const char *sql =
        "INSERT INTO resources (title, author, publisher, publication_year, isbn, category_id,"
        " total_copies, available_copies, description, added_date, is_active)"
        " VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, resource.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, resource.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, resource.getPublisher().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, resource.getPublicationYear());
    sqlite3_bind_text(stmt, 5, resource.getIsbn().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, resource.getCategoryId());
    sqlite3_bind_int(stmt, 7, resource.getTotalCopies());
    sqlite3_bind_int(stmt, 8, resource.getAvailableCopies());
    sqlite3_bind_text(stmt, 9, resource.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, resource.getAddedDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 11, resource.getIsActive() ? 1 : 0);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (success)
        resource.setResourceId((int)sqlite3_last_insert_rowid(db));
    else
        cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                     ---------- UPDATE RESOURCES ----------
   *************************************************************************  */

bool ResourceRepository::updateResource(const Resource &resource)
{
    const char *sql =
        "UPDATE resources SET "
        "title=?, author=?, publisher=?, publication_year=?, isbn=?, category_id=?, "
        "total_copies=?, available_copies=?, description=?, added_date=?, is_active=? "
        "WHERE resource_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, resource.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, resource.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, resource.getPublisher().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, resource.getPublicationYear());
    sqlite3_bind_text(stmt, 5, resource.getIsbn().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, resource.getCategoryId());
    sqlite3_bind_int(stmt, 7, resource.getTotalCopies());
    sqlite3_bind_int(stmt, 8, resource.getAvailableCopies());
    sqlite3_bind_text(stmt, 9, resource.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, resource.getAddedDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 11, resource.getIsActive() ? 1 : 0);
    sqlite3_bind_int(stmt, 12, resource.getResourceId());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success)
        cerr << "Update failed: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                     ---------- SAVE RESOURCES ----------
   *************************************************************************  */

bool ResourceRepository::save(Resource &resource)
{
    if (resource.getResourceId() == 0)
        return insertResource(resource);
    return updateResource(resource);
}

/* *************************************************************************
                     ---------- DELETE RESOURCES ----------
   *************************************************************************  */

bool ResourceRepository::deleteResource(int resourceId)
{
    const char *sql = "DELETE FROM resources WHERE resource_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, resourceId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success)
        cerr << "Delete failed: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- GET RESOURCES BY ID's ----------
   *************************************************************************  */

unique_ptr<Resource> ResourceRepository::getById(int resourceId)
{
    const char *sql =
        "SELECT resource_id, title, author, publisher, publication_year, isbn,"
        " category_id, total_copies, available_copies, description, added_date, is_active "
        "FROM resources WHERE resource_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    sqlite3_bind_int(stmt, 1, resourceId);

    unique_ptr<Resource> resource = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        resource = make_unique<Resource>(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            sqlite3_column_int(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            safeText(stmt, 9),
            safeText(stmt, 10),
            sqlite3_column_int(stmt, 11) == 1);
    }

    sqlite3_finalize(stmt);
    return resource;
}

/* *************************************************************************
                     ---------- GET ALL RESOURCES ----------
   *************************************************************************  */

vector<Resource> ResourceRepository::getAll()
{
    vector<Resource> results;

    const char *sql =
        "SELECT resource_id, title, author, publisher, publication_year, isbn,"
        "category_id, total_copies, available_copies, description, added_date, is_active "
        "FROM resources;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << sqlite3_errmsg(db) << endl;
        return results;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        results.emplace_back(
            sqlite3_column_int(stmt, 0),
            safeText(stmt, 1),
            safeText(stmt, 2),
            safeText(stmt, 3),
            sqlite3_column_int(stmt, 4),
            safeText(stmt, 5),
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            safeText(stmt, 9),
            safeText(stmt, 10),
            sqlite3_column_int(stmt, 11) == 1);
    }

    sqlite3_finalize(stmt);
    return results;
}