#include "../infrastructure/repositories/FundRequestRepository.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

bool testFundRequestRepository(sqlite3 *db)
{
    // ---------------------------------------------------------
    // 0. SETUP (Create Table)
    // ---------------------------------------------------------
    const char *createTableSQL =
        "CREATE TABLE fund_requests ("
        "request_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "requested_amount REAL, "
        "request_date TEXT, "
        "status TEXT, "
        "admin_id INTEGER, "
        "approval_date TEXT, "
        "admin_notes TEXT"
        ");";

    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "TEST FAILED [Setup]: Could not create fund_requests table. " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    FundRequestRepository repo(db);

    // ---------------------------------------------------------
    // 1. CREATE (Insert)
    // ---------------------------------------------------------
    // Arguments match the order in your repository:
    // ID (0 for new), UserID, Amount, ReqDate, Status, AdminID, ApprvDate, Notes
    FundRequest testRequest(0, 500, 15000.50, "2026-02-22", "Pending", 0, "", "");

    if (!repo.save(testRequest))
    {
        std::cerr << "TEST FAILED [Create]: Could not insert Fund Request." << std::endl;
        return false;
    }

    int generatedId = testRequest.getRequestId();
    if (generatedId <= 0)
    {
        std::cerr << "TEST FAILED [Create]: Fund Request ID was not assigned." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 2. READ (Get By ID)
    // ---------------------------------------------------------
    std::unique_ptr<FundRequest> fetchedRequest = repo.getById(generatedId);
    if (fetchedRequest == nullptr || fetchedRequest->getRequestedAmount() != 15000.50)
    {
        std::cerr << "TEST FAILED [Read]: Data mismatch or record not found." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 3. READ (Get By User ID)
    // ---------------------------------------------------------
    std::vector<FundRequest> userRequests = repo.getByUserId(500);
    if (userRequests.empty())
    {
        std::cerr << "TEST FAILED [Read By User]: No fund requests found for User 500." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 4. UPDATE (Approve the Request)
    // ---------------------------------------------------------
    // We update the status to "Approved", set an Admin ID (e.g., admin 1), and add notes.
    FundRequest approvedRequest(generatedId, 500, 15000.50, "2026-02-22", "Approved", 1, "2026-02-23", "Looks good!");
    if (!repo.save(approvedRequest))
    {
        std::cerr << "TEST FAILED [Update]: Failed to update fund request." << std::endl;
        return false;
    }

    std::unique_ptr<FundRequest> updatedRequest = repo.getById(generatedId);
    if (updatedRequest == nullptr || updatedRequest->getStatus() != "Approved" || updatedRequest->getAdminNotes() != "Looks good!")
    {
        std::cerr << "TEST FAILED [Update]: Approval update did not persist." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 5. GET ALL
    // ---------------------------------------------------------
    std::vector<FundRequest> allRequests = repo.getAllFundRequests();
    if (allRequests.empty())
    {
        std::cerr << "TEST FAILED [Get All]: Global fund requests list is empty." << std::endl;
        return false;
    }

    // ---------------------------------------------------------
    // 6. DELETE & VERIFY
    // ---------------------------------------------------------
    if (!repo.deleteFundRequest(generatedId))
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