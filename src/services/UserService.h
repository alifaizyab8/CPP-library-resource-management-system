#pragma once
#include <string>
#include <vector>
#include <memory>

// Including all Domains

#include "../domain/User.h"
#include "../domain/Resource.h"
#include "../domain/Transaction.h"
#include "../domain/Fine.h"
#include "../domain/BorrowingHistory.h"
#include "../domain/FundRequest.h"
#include "../domain/MembershipType.h"

// Forward declarations ( in this scope, only benificial for comiplation time otherwise no impact on runtime performance)

class UserRepository;
class ResourceRepository;
class TransactionRepository;
class FineRepository;
class BorrowingHistoryRepository;
class FundRequestRepository;
class MembershipTypeRepository;

// The actual UserService Class

class UserService
{
private:
    UserRepository &userRepo;
    ResourceRepository &resourceRepo;
    TransactionRepository &transactionRepo;
    FineRepository &fineRepo;
    BorrowingHistoryRepository &historyRepo;
    FundRequestRepository &fundReqRepo;
    MembershipTypeRepository &membershipRepo;

    // the helper function
    std::string toLowerCase(const std::string &str);

public:
    UserService(UserRepository &usrRepo, ResourceRepository &resRepo, TransactionRepository &tranRepo,
                FineRepository &finRepo, BorrowingHistoryRepository &brhRepo, FundRequestRepository &frRepo,
                MembershipTypeRepository &mtRepo);

    // Profile Management
    std::unique_ptr<User> getUserDetails(int userId);
    bool updateProfile(User &user);
    std::string requestAccountDeletion(int userId);

    // Catalogue
    std::vector<Resource> showAllAvailableCatalogue();
    std::vector<Resource> searchCatalogue(const std::string &keyword);

    // Borrowing & Transactions
    std::string requestToBorrow(int userId, int resourceId);
    bool cancelPendingBorrowRequest(int transactionId, int userId);

    std::vector<Transaction> getCurrentlyBorrowedResources(int userId);
    std::vector<Transaction> getPendingBorrowRequests(int userId);
    std::vector<BorrowingHistory> getBorrowingHistory(int userId);
    std::vector<Transaction> getTransactionHistory(int userId);

    // Finance Related
    std::vector<Fine> getCurrentFines(int userId);
    bool requestFund(int userId, double amount, const std::string &simulatedDate);
};