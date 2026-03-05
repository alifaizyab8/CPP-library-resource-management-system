#include "../infrastructure/repositories/FineRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testFineRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE fines ("
        "fine_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "transaction_id INTEGER, "
        "user_id INTEGER, "
        "days_overdue INTEGER, "
        "fine_amount REAL, "
        "fine_date TEXT, "
        "is_paid INTEGER, "
        "payment_date TEXT"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create fines table in RAM. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    FineRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Make sure these arguments match your Fine class constructor!
    Fine testFine(0, 101, 500, 5, 250.0, "2026-02-22", false, "");

    if (!repo.save(testFine))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Fine." << std::endl;
        return false;
    }

    int generatedId = testFine.getFineId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Fine ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<Fine> fetchedFine = repo.getById(generatedId);
    if (fetchedFine == nullptr || fetchedFine->getFineAmount() != 250.0)
    {
        std::cerr << "TEST FAILED [Read]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. READ (Get By User ID)
    // ---------------------------------------------------------
    std::vector<Fine> userFines = repo.getByUserId(500);
    if (userFines.empty())
    {
        std::cerr << "TEST FAILED [Read By User]: No fines found for User 500." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. UPDATE (Mark as Paid)
    // ---------------------------------------------------------
    Fine paidFine(generatedId, 101, 500, 5, 250.0, "2026-02-22", true, "2026-02-23");
    if (!repo.save(paidFine))
    {
        std::cerr << "TEST FAILED [Update]: Failed to mark fine as paid." << std::endl;
        return false;
    }

    std::unique_ptr<Fine> updatedFine = repo.getById(generatedId);
    if (updatedFine == nullptr || !updatedFine->getIsPaid() || updatedFine->getPaymentDate() != "2026-02-23")
    {
        std::cerr << "TEST FAILED [Update]: Payment update did not persist." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 5. GET ALL
    // ---------------------------------------------------------
    std::vector<Fine> allFines = repo.getAllFines();
    if (allFines.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Global fines list is empty." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 6. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteFine(generatedId))
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