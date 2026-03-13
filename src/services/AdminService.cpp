#include "AdminService.h"
#include "../PDFGenerator/PdfGenerator.h"
#include "../Utility/date.h"
#include <sstream>

#include "../infrastructure/repositories/UserRepository.h"
#include "../infrastructure/repositories/FineRepository.h"
#include "../infrastructure/repositories/ResourceRepository.h"
#include "../infrastructure/repositories/CategoryRepository.h"
#include "../infrastructure/repositories/FundRequestRepository.h"
#include "../infrastructure/repositories/TransactionRepository.h"
#include "../infrastructure/repositories/ReservationRepository.h"
#include "../infrastructure/repositories/MembershipTypeRepository.h"
#include "../infrastructure/repositories/BorrowingHistoryRepository.h"
// #include "../infrastructure/repositories/AdministratorRepository.h"

AdminService::AdminService(UserRepository &userRepo, FineRepository &fineRepo, ResourceRepository &resourceRepo,
                           CategoryRepository &categoryRepo, FundRequestRepository &fundRequestRepo, TransactionRepository &transactionRepo,
                           ReservationRepository &reservationRepo, MembershipTypeRepository &membershipTypeRepo,
                           BorrowingHistoryRepository &borrowingHistoryRepo)
    : userRepository(userRepo), fineRepository(fineRepo), resourceRepository(resourceRepo), categoryRepository(categoryRepo),
      fundRequestRepository(fundRequestRepo), transactionRepository(transactionRepo), reservationRepository(reservationRepo),
      membershipTypeRepository(membershipTypeRepo), borrowingHistoryRepository(borrowingHistoryRepo) {}

// Resource Management
bool AdminService::addResource(Resource &resource)
{
  return resourceRepository.save(resource);
}

bool AdminService::editResource(Resource &updatedResource)
{
  return resourceRepository.save(updatedResource);
}

bool AdminService::deleteResource(int resourceId)
{
  return resourceRepository.deleteResource(resourceId);
}

// Category Management
bool AdminService::addCategory(Category &category)
{
  return categoryRepository.save(category);
}

bool AdminService::editCategory(Category &updatedCategory)
{
  return categoryRepository.save(updatedCategory);
}

bool AdminService::deleteCategory(int categoryId)
{
  return categoryRepository.deleteCategory(categoryId);
}

// User Management
bool AdminService::addUser(User &user)

{
  return userRepository.save(user);
}

bool AdminService::editUser(User &updatedData)

{
  return userRepository.save(updatedData);
}

bool AdminService::suspendUserAccount(int userId)
{
  std::unique_ptr<User> user = userRepository.getById(userId);

  if (!user)
  {
    return false; // User not found
  }
  user->setIsActive(false);
  return userRepository.save(*user);
}

std::vector<User> AdminService::viewDeletionRequests()

{
  return userRepository.getPendingDeletionRequests();
}

bool AdminService::processAccountDeletionRequest(int userId, bool approve)

{
  std::unique_ptr<User> user = userRepository.getById(userId);

  if (!user)
  {
    return false; // User not found
  }

  if (approve)
  {
    return deleteUserAccount(userId);
  }

  else
  {
    user->setDeletionRequested(false);
    return userRepository.save(*user);
  }
}

bool AdminService::deleteUserAccount(int userId)
{
  return userRepository.deleteUser(userId);
}

// Fine Management
std::vector<Fine> AdminService::viewAllFines()
{
  return fineRepository.getAllFines();
}

std::vector<Fine> AdminService::viewFinesByUser(int userId)
{
  return fineRepository.getByUserId(userId);
}

bool AdminService::imposeFine(Fine &fine)
{
  return fineRepository.save(fine);
}

bool AdminService::markFineAsPaid(int fineId)
{
  std::unique_ptr<Fine> fine = fineRepository.getById(fineId);

  if (!fine)
  {
    return false; // Fine not found
  }

  fine->setIsPaid(true);
  return fineRepository.save(*fine);
}

bool AdminService::waiveFine(int fineId)
{
  std::unique_ptr<Fine> fine = fineRepository.getById(fineId);

  if (!fine)
  {
    return false; // Fine not found
  }
  fine->setFineAmount(0.0);
  fine->setIsPaid(true); // Mark as paid since it's waived
  return fineRepository.save(*fine);
}

bool AdminService::deleteFine(int fineId)
{
  unique_ptr<Fine> fine = fineRepository.getById(fineId);
  if (!fine)
  {
    return false; // Fine not found
  }

  return fineRepository.deleteFine(fineId);
}

// Reporting
bool AdminService::generateUserHistoryReport(const std::string &filename)
{
  // string stream to build a massive report string efficiently
  std::stringstream reportContent;

  reportContent << endl;
  reportContent << "Complete Customer Borrowing History\n";
  reportContent << "===================================\n";

  // grabing all the users
  std::vector<User> allUsers = userRepository.getAllUsers();

  if (allUsers.empty())
  {
    reportContent << "No customers found in the system.\n";
  }

  for (const User &user : allUsers)
  {

    reportContent << "-------------------------------------------------\n";
    reportContent << "Customer ID: " << user.getUserId() << "\n";
    reportContent << "Name: " << user.getFirstName() << " " << user.getLastName() << "\n";
    reportContent << "Email: " << user.getEmail() << "\n";
    reportContent << "-------------------------------------------------\n";

    // fetching their borrowing history
    std::vector<BorrowingHistory> history = borrowingHistoryRepository.getByUserId(user.getUserId());

    if (history.empty())
    {

      reportContent << "[No Borrowing History Found]\n\n";
      continue;
    }

    // fetching book titles
    for (const BorrowingHistory &record : history)
    {
      std::string bookTitle = "Unknown Resource";

      // fetching actual name
      std::unique_ptr<Resource> resource = resourceRepository.getById(record.getResourceId());
      if (resource)
      {
        bookTitle = resource->getTitle();
      }
      // Print the individual borrow record
      reportContent << "   * Borrowed: '" << bookTitle << "' (Resource ID: " << record.getResourceId() << ")\n"
                    << "     Issue Date: " << (record.getIssueDate().empty() ? "Pending" : record.getIssueDate()) << "\n"
                    << "     Due Date:   " << (record.getDueDate().empty() ? "Pending" : record.getDueDate()) << "\n"
                    << "     Returned:   " << (record.getReturnDate().empty() ? "Not Returned Yet" : record.getReturnDate()) << "\n"
                    << "     Fine Paid:  $" << record.getFineAmount() << "\n\n";
    }
  }
  std::string finalString = reportContent.str();

  // making pdf report
  makePdf(filename, "Customer Borrowing History Report", finalString);

  return true;
}

bool AdminService::generateIssuedAndOverdueReport(const std::string &filename)
{
  std::stringstream reportContent;

  reportContent << endl;
  reportContent << "Issued and Overdue Resources Report\n";
  reportContent << "===================================\n";

  // fetching all transactions
  std::vector<Transaction> activeTransactions = transactionRepository.getActiveIssues();
  if (activeTransactions.empty())
  {
    reportContent << "No active issued resources found.\n";
    makePdf(filename, "Issued and Overdue Resources Report", reportContent.str());
    return true;
  }

  std::stringstream issuedSection;
  std::stringstream overdueSection;

  int overdueCount = 0;
  int issuedCount = 0;

  // Iterating through active transactions to separate issued and overdue
  for (const Transaction &txn : activeTransactions)
  {
    std::unique_ptr<Resource> resource = resourceRepository.getById(txn.getResourceId());
    std::unique_ptr<User> user = userRepository.getById(txn.getUserId());

    std::string resourceTitle = resource ? resource->getTitle() : "Unknown Resource";
    std::string userName = (user) ? (user->getFirstName() + " " + user->getLastName()) : "Unknown User";

    // Common entry format for both sections
    std::string entry = "   * '" + resourceTitle + "' (Resource ID: " + std::to_string(txn.getResourceId()) + ")\n" + "     Borrowed by: " + userName + " (User ID: " + std::to_string(txn.getUserId()) + ")\n" + "     Issue Date: " + (txn.getIssueDate().empty() ? "Pending" : txn.getIssueDate()) + "\n" + "     Due Date:   " + (txn.getDueDate().empty() ? "Pending" : txn.getDueDate()) + "\n\n";

    if (txn.getIsOverdue())
    {
      overdueSection << entry;
      overdueCount++;
    }
    else
    {
      issuedSection << entry;
      issuedCount++;
    }
  }
  // Combining sections into final report
  reportContent << "--- OVERDUE RESOURCES (" << overdueCount << ") ---\n";
  reportContent << (overdueCount > 0 ? overdueSection.str() : "   [None]\n") << "\n";

  reportContent << "--- CURRENTLY ISSUED IN GOOD STANDING (" << issuedCount << ") ---\n";
  reportContent << (issuedCount > 0 ? issuedSection.str() : "   [None]\n") << "\n";

  // 5. Generate PDF
  makePdf(filename, "Issued and Overdue Report", reportContent.str());

  return true;
}

// Request
std::vector<Transaction> AdminService::viewPendingBorrowRequests()
{
  return transactionRepository.getbyStatus("PENDING");
}

bool AdminService::processBorrowRequest(int transactionId, bool approve)
{
  std::unique_ptr<Transaction> transaction = transactionRepository.getById(transactionId);
  if (!transaction)
  {
    return false; // Transaction not found
  }
  if (approve)
  {
    // Update transaction status to ISSUED and set issue/due dates
    transaction->setTransactionStatus("ISSUED");
    transaction->setIssueDate(getCurrentDate());
    transaction->setDueDate(getDueDate(14, transaction->getIssueDate()));

    // Decrease available copies of the resource
    std::unique_ptr<Resource> resource = resourceRepository.getById(transaction->getResourceId());
    if (resource && (resource->getIsActive()) && resource->getAvailableCopies() > 0)
    {
      resource->setAvailableCopies(resource->getAvailableCopies() - 1);
      resourceRepository.save(*resource);
    }
    else
    {
      return false; // Resource not available, cannot approve
    }

    BorrowingHistory historyRecord(
        transaction->getUserId(),     // userId
        transaction->getResourceId(), // resourceId
        transaction->getIssueDate(),  // issueDate
        transaction->getDueDate(),    // dueDate
        "",                           // returnDate (Empty because it's not returned yet)
        0.0                           // fineAmount (Starts at $0.0)
    );

    borrowingHistoryRepository.save(historyRecord);
  }
  else
  {
    // If rejected, we can either delete the transaction or just update its status to REJECTED
    transaction->setTransactionStatus("REJECTED");
  }

  return transactionRepository.updateTransaction(*transaction);
}

bool AdminService::processFundRequest(int fundRequestId, bool approve)
{
  std::unique_ptr<FundRequest> request = fundRequestRepository.getById(fundRequestId);

  if (!request || request->getStatus() != "PENDING")
  {
    return false;
  }

  request->setApprovalDate(getCurrentDate());

  if (approve)
  {
    std::unique_ptr<User> user = userRepository.getById(request->getUserId());
    if (!user)
      return false;

    double newBalance = user->getBalance() + request->getRequestedAmount();
    user->setBalance(newBalance);

    request->setStatus("APPROVED");
    request->setAdminNotes("Approved");

    if (newBalance >= 50.0 && user->getMembershipTypeId() == 1)
    {
      user->setMembershipTypeId(2);
    }

    bool userSaved = userRepository.save(*user);
    bool requestSaved = fundRequestRepository.save(*request);

    return userSaved && requestSaved;
  }
  else
  {
    request->setStatus("REJECTED");
    request->setAdminNotes("Rejected");

    return fundRequestRepository.save(*request);
  }
}

std::vector<FundRequest> AdminService::viewPendingFundRequests()
{
  return fundRequestRepository.getAllFundRequests();
}

bool AdminService::processReturn(int transactionId)
{
  std::unique_ptr<Transaction> txn = transactionRepository.getById(transactionId);

  // 1. Validate active transaction
  if (!txn || txn->getTransactionStatus() != "ISSUED")
  {
    return false;
  }

  std::string today = getCurrentDate();

  // 2. Mark transaction as returned
  txn->setTransactionStatus("RETURNED");
  txn->setIsReturned(true);
  txn->setReturnDate(today);

  // 3. Process overdue fines if applicable
  double generatedFine = 0.0;
  if (today > txn->getDueDate())
  {
    txn->setIsOverdue(true);
    generatedFine = 5.0; // Configurable late fee
    txn->setFineAmount(generatedFine);

    Fine newFine;
    newFine.setUserId(txn->getUserId());
    newFine.setFineAmount(generatedFine);
    newFine.setIsPaid(false);

    fineRepository.save(newFine);
  }

  // 4. Restore resource inventory
  std::unique_ptr<Resource> resource = resourceRepository.getById(txn->getResourceId());
  if (resource)
  {
    resource->setAvailableCopies(resource->getAvailableCopies() + 1);
    resourceRepository.save(*resource);
  }

  // 5. Update the permanent borrowing history record
  std::vector<BorrowingHistory> userHistory = borrowingHistoryRepository.getByUserId(txn->getUserId());
  for (BorrowingHistory &history : userHistory)
  {
    if (history.getResourceId() == txn->getResourceId() && history.getReturnDate().empty())
    {
      history.setReturnDate(today);
      history.setFineAmount(generatedFine);
      borrowingHistoryRepository.save(history);
      break;
    }
  }

  // 6. Persist final transaction state
  return transactionRepository.updateTransaction(*txn);
}