#include "CategoryRepository.h"
#include <sqlite3.h>
#include <iostream>
#include <memory>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

CategoryRepository::CategoryRepository(sqlite3 *connection) : db(connection) {}
CategoryRepository::~CategoryRepository() {}

/* *************************************************************************
                    ---------- INSERT CATEGORIES ----------
   *************************************************************************  */

bool CategoryRepository::insertCategory(Category &category)
{
    const char *sql = "INSERT INTO categories (name, description) VALUES (?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, category.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, category.getDescription().c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (success)
    {
        category.setCategoryId(static_cast<int>(sqlite3_last_insert_rowid(db)));
    }
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- UPDATE CATEGORIES  ----------
   *************************************************************************  */

bool CategoryRepository::updateCategory(const Category &category)
{
    const char *sql = "UPDATE categories SET name=?, description=? WHERE category_id=?;";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, category.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, category.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, category.getCategoryId());

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- SAVE CATEGORIES ----------
   *************************************************************************  */

bool CategoryRepository::save(Category &category)
{
    if (category.getCategoryId() == 0)
        return insertCategory(category);
    return updateCategory(category);
}

/* *************************************************************************
                     ---------- DELETE CATEGORIES ----------
   *************************************************************************  */

bool CategoryRepository::deleteCategory(int categoryId)
{
    const char *sql = "DELETE FROM categories WHERE category_id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, categoryId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                   ---------- GET CATEGORIES BY ID's ----------
   *************************************************************************  */

unique_ptr<Category> CategoryRepository::getById(int categoryId)
{
    const char *sql = "SELECT category_id, name, description FROM categories WHERE category_id=?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return nullptr;
    sqlite3_bind_int(stmt, 1, categoryId);

    unique_ptr<Category> category = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *desc = sqlite3_column_text(stmt, 2);
        category = make_unique<Category>(
            sqlite3_column_int(stmt, 0),
            name ? reinterpret_cast<const char *>(name) : "",
            desc ? reinterpret_cast<const char *>(desc) : "");
    }
    sqlite3_finalize(stmt);
    return category;
}

/* *************************************************************************
                  ---------- GET ALL CATEGORIES ----------
   *************************************************************************  */

vector<Category> CategoryRepository::getAll()
{
    vector<Category> results;
    const char *sql = "SELECT category_id, name, description FROM categories;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return results;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        const unsigned char *desc = sqlite3_column_text(stmt, 2);
        results.emplace_back(
            sqlite3_column_int(stmt, 0),
            name ? reinterpret_cast<const char *>(name) : "",
            desc ? reinterpret_cast<const char *>(desc) : "");
    }
    sqlite3_finalize(stmt);
    return results;
}