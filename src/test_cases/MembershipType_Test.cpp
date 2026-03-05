#include "../infrastructure/repositories/MembershipTypeRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testMembershipTypeRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE membership_types ("
        "membership_type_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "membership_name TEXT, "
        "duration_days INTEGER, "
        "price REAL, "
        "max_borrowing_limit INTEGER, "
        "borrowing_duration_days INTEGER, "
        "fine_per_day REAL, "
        "description TEXT"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create membership_types table. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    MembershipTypeRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Arguments: ID, Name, Duration, Price, MaxBooks, BorrowDuration, FinePerDay, Description
    MembershipType testPlan(0, "Gold Plan", 365, 99.99, 10, 21, 1.50, "Annual premium membership");

    if (!repo.save(testPlan))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Membership Type." << std::endl;
        return false;
    }

    int generatedId = testPlan.getMembershipTypeId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Membership Type ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<MembershipType> fetchedPlan = repo.getById(generatedId);
    if (fetchedPlan == nullptr || fetchedPlan->getMembershipName() != "Gold Plan")
    {
        std::cerr << "TEST FAILED [Read]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. UPDATE (Change Price)
    // ---------------------------------------------------------
    MembershipType updatedPlan(generatedId, "Gold Plan", 365, 119.99, 10, 21, 1.50, "Updated annual membership");
    if (!repo.save(updatedPlan))
    {
        std::cerr << "TEST FAILED [Update]: Failed to update membership type." << std::endl;
        return false;
    }

    std::unique_ptr<MembershipType> refreshedPlan = repo.getById(generatedId);
    if (refreshedPlan == nullptr || refreshedPlan->getPrice() != 119.99)
    {
        std::cerr << "TEST FAILED [Update]: Price update did not persist." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. GET ALL
    // ---------------------------------------------------------
    std::vector<MembershipType> allPlans = repo.getAllMembershipTypes();
    if (allPlans.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Global membership types list is empty." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 5. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteMembershipType(generatedId))
    {
        std::cerr << "TEST FAILED [Delete]: Could not execute delete." << std::endl;
        return false;
    }

    if (repo.getById(generatedId) != nullptr)
    {
        std::cerr << "TEST FAILED [Verify Delete]: Record still exists." << std::endl;
        return false;
    }

    return true;
}