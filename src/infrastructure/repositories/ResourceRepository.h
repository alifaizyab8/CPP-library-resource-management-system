#pragma once
#include <vector>
#include <memory>
#include <string>
#include "sqlite3.h"
#include "../../domain/Resource.h"

class ResourceRepository {
private:
    sqlite3* db;
    bool insertResource(Resource& resource);
    bool updateResource(const Resource& resource);

public:
    explicit ResourceRepository(sqlite3* connection);
    ~ResourceRepository();

    bool save(Resource& resource);
    bool deleteResource(int resourceId);
    std::unique_ptr<Resource> getById(int resourceId);
    std::vector<Resource> getAll();
};