#include "ResourceRepository.h"
#include <iostream>

using namespace std;

ResourceRepository::ResourceRepository(sqlite3 *connection) : db(connection) {}
ResourceRepository::~ResourceRepository() {}

bool ResourceRepository::insertResource(Resource &resource)
{
    const char *sql = "INSERT INTO resources (title, author, publisher, publication_year, isbn, category_id, "
                      "resource_type_id, total_copies, available_copies, description, added_date, is_active) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, resource.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, resource.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, resource.getPublisher().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, resource.getPublicationYear());
    sqlite3_bind_text(stmt, 5, resource.getIsbn().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, resource.getCategoryId());     // Foreign Key
    sqlite3_bind_int(stmt, 7, resource.getResourceTypeId()); // Foreign Key
    sqlite3_bind_int(stmt, 8, resource.getTotalCopies());
    sqlite3_bind_int(stmt, 9, resource.getAvailableCopies());
    sqlite3_bind_text(stmt, 10, resource.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, resource.getAddedDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 12, resource.getIsActive() ? 1 : 0);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (success)
    {
        resource.setResourceId(static_cast<int>(sqlite3_last_insert_rowid(db)));
    }
    else
    {
        cerr << "Resource Insert Failed (Check FK constraints): " << sqlite3_errmsg(db) << endl;
    }
    sqlite3_finalize(stmt);
    return success;
}

bool ResourceRepository::updateResource(const Resource &resource)
{
    const char *sql = "UPDATE resources SET title=?, author=?, publisher=?, publication_year=?, isbn=?, category_id=?, "
                      "resource_type_id=?, total_copies=?, available_copies=?, description=?, added_date=?, is_active=? "
                      "WHERE resource_id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, resource.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, resource.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, resource.getPublisher().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, resource.getPublicationYear());
    sqlite3_bind_text(stmt, 5, resource.getIsbn().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, resource.getCategoryId());
    sqlite3_bind_int(stmt, 7, resource.getResourceTypeId());
    sqlite3_bind_int(stmt, 8, resource.getTotalCopies());
    sqlite3_bind_int(stmt, 9, resource.getAvailableCopies());
    sqlite3_bind_text(stmt, 10, resource.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, resource.getAddedDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 12, resource.getIsActive() ? 1 : 0);
    sqlite3_bind_int(stmt, 13, resource.getResourceId());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool ResourceRepository::save(Resource &resource)
{
    if (resource.getResourceId() == 0)
        return insertResource(resource);
    return updateResource(resource);
}

bool ResourceRepository::deleteResource(int resourceId)
{
    const char *sql = "DELETE FROM resources WHERE resource_id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, resourceId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

unique_ptr<Resource> ResourceRepository::getById(int resourceId)
{
    const char *sql = "SELECT * FROM resources WHERE resource_id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return nullptr;
    sqlite3_bind_int(stmt, 1, resourceId);

    unique_ptr<Resource> resource = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        resource = make_unique<Resource>(
            sqlite3_column_int(stmt, 0),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
            sqlite3_column_int(stmt, 4),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 11)),
            sqlite3_column_int(stmt, 12) == 1);
    }
    sqlite3_finalize(stmt);
    return resource;
}

vector<Resource> ResourceRepository::getAll()
{
    vector<Resource> results;
    const char *sql = "SELECT * FROM resources;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return results;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        results.emplace_back(
            sqlite3_column_int(stmt, 0),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
            sqlite3_column_int(stmt, 4),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
            sqlite3_column_int(stmt, 6),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 11)),
            sqlite3_column_int(stmt, 12) == 1);
    }
    sqlite3_finalize(stmt);
    return results;
}