#include "../infrastructure/repositories/UserRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testUserRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE users ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "first_name TEXT NOT NULL, "
        "last_name TEXT NOT NULL, "
        "email TEXT NOT NULL, "
        "address TEXT, "
        "phone TEXT, "
        "balance REAL, "
        "membership_type_id INTEGER, "
        "registration_date TEXT, "
        "is_active INTEGER"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create users table. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    UserRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Arguments: ID, Username, Password, FirstName, LastName, Email, Address, Phone, Balance, MembershipID, RegDate, isActive
    User testUser(0, "waqar_student", "securepass", "Waqar", "Wasif", "waqar@test.com", "Campus Res", "03000000000", 0.0, 1, "2026-02-22", true);

    if (!repo.save(testUser))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert User." << std::endl;
        return false;
    }

    int generatedId = testUser.getUserId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: User ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<User> fetchedUser = repo.getById(generatedId);
    if (fetchedUser == nullptr || fetchedUser->getUsername() != "waqar_student")
    {
        std::cerr << "TEST FAILED [Read by ID]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. READ (Get By Username)
    // ---------------------------------------------------------
    std::unique_ptr<User> fetchedByUsername = repo.getByUsername("waqar_student");
    if (fetchedByUsername == nullptr || fetchedByUsername->getUserId() != generatedId)
    {
        std::cerr << "TEST FAILED [Read by Username]: Could not fetch correct user by username." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. UPDATE (Add Balance & Change Address)
    // ---------------------------------------------------------
    User updatedUser(generatedId, "waqar_student", "securepass", "Waqar", "Wasif", "waqar@test.com", "New Campus Res", "03000000000", 50.50, 1, "2026-02-22", true);

    if (!repo.save(updatedUser))
    {
        std::cerr << "TEST FAILED [Update]: Failed to update user." << std::endl;
        return false;
    }

    std::unique_ptr<User> refreshedUser = repo.getById(generatedId);
    if (refreshedUser == nullptr || refreshedUser->getBalance() != 50.50 || refreshedUser->getAddress() != "New Campus Res")
    {
        std::cerr << "TEST FAILED [Update]: Balance or Address update did not persist." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 5. GET ALL
    // ---------------------------------------------------------
    std::vector<User> allUsers = repo.getAllUsers();
    if (allUsers.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Global users list is empty." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 6. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteUser(generatedId))
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