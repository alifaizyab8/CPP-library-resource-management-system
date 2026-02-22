#include "../infrastructure/repositories/AdministratorRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Test function for AdministratorRepository
bool testAdministratorRepository(sqlite3 *db)
{

    // 0. SETUP (Create Table)
    const char *createTableSQL =
        "CREATE TABLE administrators ("
        "admin_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL, "
        "password TEXT NOT NULL, "
        "first_name TEXT NOT NULL, "
        "last_name TEXT NOT NULL, "
        "email TEXT NOT NULL, "
        "created_date TEXT NOT NULL, "
        "is_active INTEGER NOT NULL"
        ");";
    char *errMsg = nullptr;
    sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (errMsg)
        sqlite3_free(errMsg);

    AdministratorRepository repo(db);

    // 1. CREATE (Insert)
    Administrator testAdmin(0, "test_admin", "secure123", "Waqar", "Wasif",
                            "waqar@example.com", "2026-02-22", true);

    if (!repo.save(testAdmin))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Administrator." << std::endl;
        return false;
    }

    // After a successful insert, the save() method should have updated the ID.
    int generatedId = testAdmin.getAdminId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Administrator ID was not assigned by the database." << std::endl;
        return false;
    }

    // 2. READ (Select)
    // Fetching the exact same record from the database.
    std::unique_ptr<Administrator> fetchedAdmin = repo.getById(generatedId);

    if (fetchedAdmin == nullptr)
    {
        std::cerr << "TEST FAILED [Read]: Could not fetch inserted Administrator." << std::endl;
        return false;
    }

    if (fetchedAdmin->getUsername() != "test_admin" || fetchedAdmin->getFirstName() != "Waqar")
    {
        std::cerr << "TEST FAILED [Read]: Fetched data does not match the inserted data." << std::endl;
        return false;
    }

    // 3. UPDATE
    // Since there are no setters, we test the update by creating a new object
    // that uses the exact same generated ID, but changes the first name to "Javed".
    Administrator updatedData(generatedId, "test_admin", "secure123", "Javed", "Wasif",
                              "waqar@example.com", "2026-02-22", true);

    if (!repo.save(updatedData))
    {
        std::cerr << "TEST FAILED [Update]: Could not execute update." << std::endl;
        return false;
    }

    // Re-fetch to ensure the change was permanently saved to the database.
    std::unique_ptr<Administrator> updatedAdmin = repo.getById(generatedId);
    if (updatedAdmin == nullptr || updatedAdmin->getFirstName() != "Javed")
    {
        std::cerr << "TEST FAILED [Update]: The updated first name did not persist in the database." << std::endl;
        return false;
    }

    // GET ALL (Select Multiple)
    // Fetch all admins. As we have created only one, the list should contain exactly that one record.
    std::vector<Administrator> allAdmins = repo.getAllAdministrators();

    if (allAdmins.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Returned an empty list, but our test admin should be there." << std::endl;
        return false;
    }

    bool foundOurGuy = false;
    for (const auto &a : allAdmins)
    {
        if (a.getAdminId() == generatedId)
        {
            foundOurGuy = true;
            break;
        }
    }

    if (!foundOurGuy)
    {
        std::cerr << "TEST FAILED [Get All]: The test admin was not found in the full list." << std::endl;
        return false;
    }

    // 4. DELETE
    // Attempt to delete the record.
    if (!repo.deleteAdministrator(generatedId))
    {
        std::cerr << "TEST FAILED [Delete]: Could not execute delete operation." << std::endl;
        return false;
    }

    // 5. VERIFY DELETION
    // Try to fetch the deleted record. It MUST return nullptr.
    std::unique_ptr<Administrator> deletedAdmin = repo.getById(generatedId);
    if (deletedAdmin != nullptr)
    {
        std::cerr << "TEST FAILED [Verify Delete]: Administrator still exists in the database after deletion." << std::endl;
        return false;
    }

    // If it made it through all 6 steps without returning false, the repository is flawless!
    return true;
}