#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/MembershipType.h"

class MembershipTypeRepository {
private:
    sqlite3* db;

    bool insertMembershipType(const MembershipType& type);
    bool updateMembershipType(const MembershipType& type);

public:
    explicit MembershipTypeRepository(sqlite3* connection);
    ~MembershipTypeRepository();

    bool save(const MembershipType& type);
    bool deleteMembershipType(int typeId);

    std::unique_ptr<MembershipType> getById(int typeId);
    std::vector<MembershipType> getAllMembershipTypes();
};