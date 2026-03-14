#pragma once

#include <string>
#include <vector>
#include <memory>

// Include domain models 
#include "../domain/User.h"
#include "../domain/Fine.h"
#include "../domain/Resource.h"
#include "../domain/Category.h"
#include "../domain/FundRequest.h"
#include "../domain/Transaction.h"
#include "../domain/Reservation.h"
#include "../domain/Administrator.h"
#include "../domain/MembershipType.h"
#include "../domain/BorrowingHistory.h"

// Forward declarations of repositories (IWYU principle)
class UserRepository;
class FineRepository;
class ResourceRepository;
class CategoryRepository;
class FundRequestRepository;
class TransactionRepository;
class ReservationRepository;
//class AdministratorRepository;
class MembershipTypeRepository;
class BorrowingHistoryRepository;


class AdminService{

    private:
        UserRepository &userRepository;
        FineRepository &fineRepository;
        ResourceRepository &resourceRepository;
        CategoryRepository &categoryRepository;
        FundRequestRepository &fundRequestRepository;
        TransactionRepository &transactionRepository;
        ReservationRepository &reservationRepository;
       // AdministratorRepository &administratorRepository; // for any admin-specific data management, if needed in the future
        MembershipTypeRepository &membershipTypeRepository; 
        BorrowingHistoryRepository &borrowingHistoryRepository;

    public:
        AdminService(UserRepository &userRepo, FineRepository &fineRepo, ResourceRepository &resourceRepo, 
                     CategoryRepository &categoryRepo, FundRequestRepository &fundRequestRepo, TransactionRepository &transactionRepo,
                     ReservationRepository &reservationRepo, MembershipTypeRepository &membershipTypeRepo,
                     BorrowingHistoryRepository &borrowingHistoryRepo);


        // Catalog & Category Management
        bool deleteResource(int resourceId);
        bool addResource(Resource &resource);
        bool editResource(Resource &updatedResource);

        bool deleteCategory(int categoryId);
        bool addCategory(Category &category);
        bool editCategory(Category &updatedCategory);

        // Request Approvals
        std::vector<Transaction> viewPendingBorrowRequests();
        std::vector<FundRequest> viewPendingFundRequests();
        bool processBorrowRequest(int transactionId, bool approve, std::string &dateToday);
        bool processAccountDeletionRequest(int userId, bool approve);

        // User Management
        bool addUser(User &user);
        bool editUser(User &updatedData);
        bool deleteUserAccount(int userId);
        bool suspendUserAccount(int userId);
        bool reactivateUserAccount(int userId);
        std::vector<User> viewDeletionRequests();

        // Fine Management 
        std::vector<Fine> viewAllFines();
        std::vector<Fine> viewFinesByUser(int userId);
        bool imposeFine(Fine &fine);
        bool markFineAsPaid(int fineId);
        bool waiveFine(int fineId);
        bool deleteFine(int fineId);
        bool updateFine(Fine &fine);
        void updateDailyFines(const std::string &simulatedToday);


        // Reporting
        // (Depending on our design, these might return complex string reports or vectors of data)
        bool generateUserHistoryReport(const std::string& filename);
        bool generateIssuedAndOverdueReport(const std::string &filename);

        // Process
        bool processFundRequest(int fundRequestId, bool approve, std::string &dateToday);
        bool processReturn(int transactionId, std::string &dateToday);
};


