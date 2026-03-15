#include "UserService.h"
#include <cctype>
#include <string>
#include <vector>
#include <memory>
// Include all repositories
#include "../infrastructure/repositories/UserRepository.h"
#include "../infrastructure/repositories/ResourceRepository.h"
#include "../infrastructure/repositories/TransactionRepository.h"
#include "../infrastructure/repositories/FineRepository.h"
#include "../infrastructure/repositories/BorrowingHistoryRepository.h"
#include "../infrastructure/repositories/FundRequestRepository.h"
#include "../infrastructure/repositories/MembershipTypeRepository.h"

UserService::UserService(UserRepository &usrRepo, ResourceRepository &resRepo,
                         TransactionRepository &trRepo, FineRepository &finRepo,
                         BorrowingHistoryRepository &brhRepo, FundRequestRepository &frRepo,
                         MembershipTypeRepository &mtRepo)
    : userRepo(usrRepo), resourceRepo(resRepo), transactionRepo(trRepo),
      fineRepo(finRepo), historyRepo(brhRepo), fundReqRepo(frRepo), membershipRepo(mtRepo) {}

bool UserService::updateProfile(User &user)
{
    return userRepo.save(user);
}
std::string UserService::toLowerCase(const std::string &str)
{
    std::string lowerStr = str;

    // for every character c
    for (char &c : lowerStr)
    {
        c = std::tolower(c);
    }

    return lowerStr;
}
std::string UserService::requestAccountDeletion(int userId)
{
    // Unpaid Fine Check
    std::vector<Fine> userFines = fineRepo.getByUserId(userId);
    for (const auto &fine : userFines)
    {
        if (!fine.getIsPaid())
        {
            return "Cannot request deletion: You have unpaid fines. Please clear your dues first.";
        }
    }

    // Active borrowal check
    std::vector<Transaction> userTransactions = transactionRepo.getByUserId(userId);
    for (const auto &txn : userTransactions)
    {
        if (!txn.getIsReturned() &&
            (txn.getTransactionStatus() == "PENDING" || txn.getTransactionStatus() == "ISSUED"))
        {
            return "Cannot request deletion: You have active borrowed resources. Please return them first.";
        }
    }

    // No pending dues or borrowals
    std::unique_ptr<User> user = userRepo.getById(userId);
    if (user != nullptr)
    {
        user->setDeletionRequested(true);
        if (userRepo.save(*user))
        {
            return "Account deletion request submitted successfully. Waiting for Admin approval.";
        }
    }

    return "Some error occurred while requesting account deletion";
}

std::vector<Resource> UserService::showAllAvailableCatalogue()
{
    std::vector<Resource> all = resourceRepo.getAll();
    std::vector<Resource> available;
    // Checks if active or not & available copies are greater than zero
    for (const auto &resource : all)
    {
        if (resource.getIsActive() && resource.getAvailableCopies() > 0)
        {
            available.push_back(resource);
        }
    }
    return available;
}

std::vector<Resource> UserService::searchCatalogue(const std::string &keyword)
{
    std::vector<Resource> allResources = resourceRepo.getAll();
    std::vector<Resource> matchedResources;
    std::string searchKey = toLowerCase(keyword);

    for (const auto &res : allResources)
    {
        if (!res.getIsActive())
            continue;

        std::string titleLower = toLowerCase(res.getTitle());
        std::string authorLower = toLowerCase(res.getAuthor());

        // Check if keyword exists in title or author
        if (titleLower.find(searchKey) != std::string::npos ||
            authorLower.find(searchKey) != std::string::npos)
        {
            matchedResources.push_back(res);
        }
    }
    return matchedResources;
}
std::string UserService::requestToBorrow(int userId, int resourceId)
{
    // Simple resource Checks
    std::unique_ptr<Resource> resource = resourceRepo.getById(resourceId);
    if (!resource || !resource->getIsActive() || resource->getAvailableCopies() <= 0)
    {
        return "Resource is currently unavailable.";
    }

    // User has any unpaid fines
    std::vector<Fine> userFines = fineRepo.getByUserId(userId);
    for (const auto &fine : userFines)
    {
        if (!fine.getIsPaid())
        {
            return "Cannot borrow: You have unpaid overdue fines.";
        }
    }

    // If limit reached of max borrowals
    std::unique_ptr<User> user = userRepo.getById(userId);
    if (!user)
    {
        return "User not found.";
    }
    std::unique_ptr<MembershipType> membership = membershipRepo.getById(user->getMembershipTypeId());
    int maxLimit = (membership != nullptr) ? membership->getMaxBorrowingLimit() : 2; // Default to 2 if missing

    std::vector<Transaction> userTransactions = transactionRepo.getByUserId(userId);
    int activeBorrows = 0;
    for (const auto &txn : userTransactions)
    {
        // Count active pending requests AND currently issued items
        if (!txn.getIsReturned() &&
            (txn.getTransactionStatus() == "PENDING" || txn.getTransactionStatus() == "ISSUED"))
        {
            activeBorrows++;
        }
    }

    if (activeBorrows >= maxLimit)
    {
        return "Cannot borrow: You have reached your maximum borrowing limit.";
    }

    // now creating a transaction object for admin to approve
    Transaction newBorrowRequest;
    newBorrowRequest.setUserId(userId);
    newBorrowRequest.setResourceId(resourceId);
    newBorrowRequest.setIssueDate(""); // Admin assigns on approval
    newBorrowRequest.setDueDate("");   // Admin assigns on approval
    newBorrowRequest.setReturnDate("");
    newBorrowRequest.setFineAmount(0.0);
    newBorrowRequest.setIsReturned(false);
    newBorrowRequest.setIsOverdue(false);
    newBorrowRequest.setRenewalCount(0);
    newBorrowRequest.setTransactionStatus("PENDING");

    if (transactionRepo.save(newBorrowRequest))
    {
        return "Borrow request submitted successfully! Waiting for Admin approval.";
    }
    return "System error: Could not process request.";
}

std::vector<BorrowingHistory> UserService::getBorrowingHistory(int userId)
{
    return historyRepo.getByUserId(userId);
}
std::vector<Transaction> UserService::getTransactionHistory(int userId)
{
    return transactionRepo.getByUserId(userId);
}
// Current is the same as Unpaid
std::vector<Fine> UserService::getCurrentFines(int userId)
{
    std::vector<Fine> all = fineRepo.getByUserId(userId);
    std::vector<Fine> unpaid;
    for (const auto &fine : all)
    {
        if (!fine.getIsPaid())
        {
            unpaid.push_back(fine);
        }
    }
    return unpaid;
}

bool UserService::requestFund(int userId, double amount, const std::string &simualtedDate)
{
    if (amount <= 0)
        return false;

    // Creates a request for the Admin to verify
    FundRequest request;
    request.setUserId(userId);
    request.setRequestedAmount(amount);
    request.setRequestDate(simualtedDate);
    request.setStatus("PENDING");

    return fundReqRepo.save(request);
}

// Getthe User
std::unique_ptr<User> UserService::getUserDetails(int userId)
{
    return userRepo.getById(userId);
}

// get currently owned or borrowed resources
std::vector<Transaction> UserService::getCurrentlyBorrowedResources(int userId)
{
    std::vector<Transaction> all = transactionRepo.getByUserId(userId);
    std::vector<Transaction> owned;
    for (const auto &txn : all)
    {
        if (txn.getTransactionStatus() == "ISSUED" && !txn.getIsReturned())
        {
            owned.push_back(txn);
        }
    }
    return owned;
}

// get pending borrowals
std::vector<Transaction> UserService::getPendingBorrowRequests(int userId)
{
    std::vector<Transaction> all = transactionRepo.getByUserId(userId);
    std::vector<Transaction> pending;
    for (const auto &txn : all)
    {
        if (txn.getTransactionStatus() == "PENDING")
        {
            pending.push_back(txn);
        }
    }
    return pending;
}

// cancel a pending request
bool UserService::cancelPendingBorrowRequest(int transactionId, int userId)
{
    std::unique_ptr<Transaction> txn = transactionRepo.getById(transactionId);

    // 3 checks: transaction exists, belongs to the same user and has status of pending
    if (txn != nullptr && txn->getUserId() == userId && txn->getTransactionStatus() == "PENDING")
    {
        txn->setTransactionStatus("CANCELLED");
        return transactionRepo.save(*txn);
    }
    return false;
}