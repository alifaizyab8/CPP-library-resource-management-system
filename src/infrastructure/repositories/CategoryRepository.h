#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include <memory>
#include "../../domain/Category.h"

class CategoryRepository
{
private:
    sqlite3 *db;
    bool insertCategory(Category &category);
    bool updateCategory(const Category &category);

public:
    explicit CategoryRepository(sqlite3 *connection);
    ~CategoryRepository();

    bool save(Category &category);
    bool deleteCategory(int categoryId);
    std::unique_ptr<Category> getById(int categoryId);
    std::vector<Category> getAll();
};