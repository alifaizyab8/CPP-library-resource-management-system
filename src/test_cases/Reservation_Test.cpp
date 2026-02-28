#include "../infrastructure/repositories/ReservationRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testReservationRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE reservations ("
        "reservation_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "resource_id INTEGER, "
        "reservation_date TEXT, "
        "expiry_date TEXT, "
        "is_fulfilled INTEGER, "
        "is_cancelled INTEGER, "
        "status TEXT"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create reservations table. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    ReservationRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Arguments: ID, UserID, ResourceID, ResDate, ExpiryDate, isFulfilled, isCancelled, Status
    Reservation testRes(0, 500, 1001, "2026-02-22", "2026-02-25", false, false, "Active");

    if (!repo.save(testRes))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Reservation." << std::endl;
        return false;
    }

    int generatedId = testRes.getReservationId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Reservation ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<Reservation> fetchedRes = repo.getById(generatedId);
    if (fetchedRes == nullptr || fetchedRes->getResourceId() != 1001)
    {
        std::cerr << "TEST FAILED [Read]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. READ (Get By User ID & Resource ID)
    // ---------------------------------------------------------
    std::vector<Reservation> userRes = repo.getByUserId(500);
    if (userRes.empty())
    {
        std::cerr << "TEST FAILED [Read By User]: No reservations found for User 500." << std::endl;
        return false;
    }

    std::vector<Reservation> resourceRes = repo.getByResourceId(1001);
    if (resourceRes.empty())
    {
        std::cerr << "TEST FAILED [Read By Resource]: No reservations found for Resource 1001." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. UPDATE (Mark as Fulfilled)
    // ---------------------------------------------------------
    Reservation fulfilledRes(generatedId, 500, 1001, "2026-02-22", "2026-02-25", true, false, "Fulfilled");
    if (!repo.save(fulfilledRes))
    {
        std::cerr << "TEST FAILED [Update]: Failed to update reservation." << std::endl;
        return false;
    }

    std::unique_ptr<Reservation> updatedRes = repo.getById(generatedId);
    if (updatedRes == nullptr || !updatedRes->getIsFulfilled() || updatedRes->getStatus() != "Fulfilled")
    {
        std::cerr << "TEST FAILED [Update]: Fulfillment update did not persist." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 5. GET ALL
    // ---------------------------------------------------------
    std::vector<Reservation> allRes = repo.getAllReservations();
    if (allRes.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Global reservations list is empty." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 6. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteReservation(generatedId))
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