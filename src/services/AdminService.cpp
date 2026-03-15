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

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   ************************************************************************* */

AdminService::AdminService(UserRepository &userRepo, FineRepository &fineRepo, ResourceRepository &resourceRepo,
                           CategoryRepository &categoryRepo, FundRequestRepository &fundRequestRepo, TransactionRepository &transactionRepo,
                           ReservationRepository &reservationRepo, MembershipTypeRepository &membershipTypeRepo,
                           BorrowingHistoryRepository &borrowingHistoryRepo)
    : userRepository(userRepo), fineRepository(fineRepo), resourceRepository(resourceRepo), categoryRepository(categoryRepo),
      fundRequestRepository(fundRequestRepo), transactionRepository(transactionRepo), reservationRepository(reservationRepo),
      membershipTypeRepository(membershipTypeRepo), borrowingHistoryRepository(borrowingHistoryRepo) {}

/* *************************************************************************
                 ---------- RESOURCE MANAGEMENT ----------
   ************************************************************************* */

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

std::unique_ptr<Resource> AdminService::getResourceById(int resourceId)
{
  return resourceRepository.getById(resourceId);
}

/* *************************************************************************
                 ---------- CATEGORY MANAGEMENT ----------
   ************************************************************************* */

std::unique_ptr<Category> AdminService::getCategoryById(int categoryId)
{
  return categoryRepository.getById(categoryId);
}

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

/* *************************************************************************
                 ---------- USER MANAGEMENT ----------
   ************************************************************************* */

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
    return false;
  }
  user->setIsActive(false);
  return userRepository.save(*user);
}

bool AdminService::reactivateUserAccount(int userId)
{
  std::unique_ptr<User> user = userRepository.getById(userId);
  if (!user)
    return false;

  user->setIsActive(true);
  return userRepository.save(*user);
}

std::vector<User> AdminService::viewAllUsers()
{
  return userRepository.getAllUsers();
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
    return false;
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

std::unique_ptr<User> AdminService::getUserById(int userId)
{
  return userRepository.getById(userId);
}

/* *************************************************************************
                 ---------- FINE MANAGEMENT ----------
   ************************************************************************* */

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
    return false;
  }

  fine->setIsPaid(true);
  return fineRepository.save(*fine);
}

bool AdminService::waiveFine(int fineId)
{
  std::unique_ptr<Fine> fine = fineRepository.getById(fineId);

  if (!fine)
    return false;

  transactionRepository.beginTransaction();

  fine->setFineAmount(0.0);
  fine->setIsPaid(true);

  if (!fineRepository.save(*fine))
  {
    transactionRepository.rollbackTransaction();
    return false;
  }

  std::unique_ptr<Transaction> txn = transactionRepository.getById(fine->getTransactionId());
  if (txn)
  {
    txn->setFineAmount(0.0);
    if (!transactionRepository.updateTransaction(*txn))
    {
      transactionRepository.rollbackTransaction();
      return false;
    }
  }

  transactionRepository.commitTransaction();
  return true;
}

void AdminService::updateDailyFines(const std::string &dateToday)
{
  std::vector<Transaction> activeTransactions = transactionRepository.getActiveIssues();

  for (Transaction &txn : activeTransactions)
  {
    if (dateToday > txn.getDueDate())
    {
      txn.setIsOverdue(true);

      int daysLate = calculateDaysOverdue(txn.getDueDate(), dateToday);

      if (daysLate > 0)
      {
        double currentFineAmount = daysLate * 5.0;

        bool fineExists = false;
        std::vector<Fine> userFines = fineRepository.getByUserId(txn.getUserId());

        for (Fine &existingFine : userFines)
        {
          if (existingFine.getTransactionId() == txn.getTransactionId())
          {
            existingFine.setDaysOverdue(daysLate);
            existingFine.setFineAmount(currentFineAmount);
            existingFine.setFineDate(dateToday);
            fineRepository.save(existingFine);
            fineExists = true;
            break;
          }
        }

        if (!fineExists)
        {
          Fine newFine;
          newFine.setTransactionId(txn.getTransactionId());
          newFine.setUserId(txn.getUserId());
          newFine.setDaysOverdue(daysLate);
          newFine.setFineAmount(currentFineAmount);
          newFine.setFineDate(dateToday);
          newFine.setIsPaid(false);

          fineRepository.save(newFine);
        }

        txn.setFineAmount(currentFineAmount);
        transactionRepository.updateTransaction(txn);
      }
    }
  }
}

/* *************************************************************************
                 ---------- REPORTING ----------
   ************************************************************************* */

bool AdminService::generateUserHistoryReport(const std::string &filename)
{
  std::stringstream reportContent;

  reportContent << endl;
  reportContent << "Complete Customer Borrowing History\n";
  reportContent << "===================================\n";

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

    std::vector<BorrowingHistory> history = borrowingHistoryRepository.getByUserId(user.getUserId());

    if (history.empty())
    {

      reportContent << "[No Borrowing History Found]\n\n";
      continue;
    }

    for (const BorrowingHistory &record : history)
    {
      std::string bookTitle = "Unknown Resource";

      std::unique_ptr<Resource> resource = resourceRepository.getById(record.getResourceId());
      if (resource)
      {
        bookTitle = resource->getTitle();
      }
      reportContent << "   * Borrowed: '" << bookTitle << "' (Resource ID: " << record.getResourceId() << ")\n"
                    << "     Issue Date: " << (record.getIssueDate().empty() ? "Pending" : record.getIssueDate()) << "\n"
                    << "     Due Date:   " << (record.getDueDate().empty() ? "Pending" : record.getDueDate()) << "\n"
                    << "     Returned:   " << (record.getReturnDate().empty() ? "Not Returned Yet" : record.getReturnDate()) << "\n"
                    << "     Fine Paid:  $" << record.getFineAmount() << "\n\n";
    }
  }
  std::string finalString = reportContent.str();

  makePdf(filename, "Customer Borrowing History Report", finalString);

  return true;
}

bool AdminService::generateIssuedAndOverdueReport(const std::string &filename)
{
  std::stringstream reportContent;

  reportContent << endl;
  reportContent << "Issued and Overdue Resources Report\n";
  reportContent << "===================================\n";

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

  for (const Transaction &txn : activeTransactions)
  {
    std::unique_ptr<Resource> resource = resourceRepository.getById(txn.getResourceId());
    std::unique_ptr<User> user = userRepository.getById(txn.getUserId());

    std::string resourceTitle = resource ? resource->getTitle() : "Unknown Resource";
    std::string userName = (user) ? (user->getFirstName() + " " + user->getLastName()) : "Unknown User";

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

  reportContent << "--- OVERDUE RESOURCES (" << overdueCount << ") ---\n";
  reportContent << (overdueCount > 0 ? overdueSection.str() : "   [None]\n") << "\n";

  reportContent << "--- CURRENTLY ISSUED IN GOOD STANDING (" << issuedCount << ") ---\n";
  reportContent << (issuedCount > 0 ? issuedSection.str() : "   [None]\n") << "\n";

  makePdf(filename, "Issued and Overdue Report", reportContent.str());

  return true;
}

/* *************************************************************************
                 ---------- TRANSACTION PROCESSING ----------
   ************************************************************************* */

std::vector<Transaction> AdminService::viewPendingBorrowRequests()
{
  return transactionRepository.getbyStatus("PENDING");
}

bool AdminService::processBorrowRequest(int transactionId, bool approve, std::string &dateToday)
{
  std::unique_ptr<Transaction> transaction = transactionRepository.getById(transactionId);
  if (!transaction)
    return false;

  transactionRepository.beginTransaction();

  if (approve)
  {
    transaction->setTransactionStatus("ISSUED");
    transaction->setIssueDate(dateToday);
    transaction->setDueDate(getDueDate(14, dateToday));

    std::unique_ptr<Resource> resource = resourceRepository.getById(transaction->getResourceId());
    if (resource && resource->getIsActive() && resource->getAvailableCopies() > 0)
    {
      resource->setAvailableCopies(resource->getAvailableCopies() - 1);

      if (!resourceRepository.save(*resource))
      {
        transactionRepository.rollbackTransaction();
        return false;
      }
    }
    else
    {
      transactionRepository.rollbackTransaction();
      return false;
    }

    BorrowingHistory historyRecord(
        transaction->getUserId(), transaction->getResourceId(),
        transaction->getIssueDate(), transaction->getDueDate(), "", 0.0);

    if (!borrowingHistoryRepository.save(historyRecord))
    {
      transactionRepository.rollbackTransaction();
      return false;
    }
  }
  else
  {
    transaction->setTransactionStatus("REJECTED");
  }

  if (transactionRepository.updateTransaction(*transaction))
  {
    transactionRepository.commitTransaction();
    return true;
  }
  else
  {
    transactionRepository.rollbackTransaction();
    return false;
  }
}

bool AdminService::processReturn(int transactionId, std::string &dateToday)
{
  std::unique_ptr<Transaction> txn = transactionRepository.getById(transactionId);

  if (!txn || txn->getTransactionStatus() != "ISSUED")
  {
    return false;
  }

  transactionRepository.beginTransaction();

  std::string today = dateToday;

  txn->setTransactionStatus("RETURNED");
  txn->setIsReturned(true);
  txn->setReturnDate(today);

  std::unique_ptr<Resource> resource = resourceRepository.getById(txn->getResourceId());
  if (resource)
  {
    resource->setAvailableCopies(resource->getAvailableCopies() + 1);

    if (!resourceRepository.save(*resource))
    {
      transactionRepository.rollbackTransaction();
      return false;
    }
  }

  std::vector<BorrowingHistory> userHistory = borrowingHistoryRepository.getByUserId(txn->getUserId());
  for (BorrowingHistory &history : userHistory)
  {
    if (history.getResourceId() == txn->getResourceId() && history.getReturnDate().empty())
    {
      history.setReturnDate(today);
      history.setFineAmount(txn->getFineAmount());

      if (!borrowingHistoryRepository.save(history))
      {
        transactionRepository.rollbackTransaction();
        return false;
      }
      break;
    }
  }

  if (transactionRepository.updateTransaction(*txn))
  {
    transactionRepository.commitTransaction();
    return true;
  }
  else
  {
    transactionRepository.rollbackTransaction();
    return false;
  }
}

/* *************************************************************************
                 ---------- FUND REQUEST PROCESSING ----------
   ************************************************************************* */

std::vector<FundRequest> AdminService::viewPendingFundRequests()
{
  return fundRequestRepository.getAllFundRequests();
}

bool AdminService::processFundRequest(int fundRequestId, bool approve, std::string &dateToday)
{
  std::unique_ptr<FundRequest> request = fundRequestRepository.getById(fundRequestId);

  if (!request || request->getStatus() != "PENDING")
    return false;

  request->setApprovalDate(dateToday);

  transactionRepository.beginTransaction();

  if (approve)
  {
    std::unique_ptr<User> user = userRepository.getById(request->getUserId());
    if (!user)
    {
      transactionRepository.rollbackTransaction();
      return false;
    }

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

    if (userSaved && requestSaved)
    {
      transactionRepository.commitTransaction();
      return true;
    }
    else
    {
      transactionRepository.rollbackTransaction();
      return false;
    }
  }
  else
  {
    request->setStatus("REJECTED");
    request->setAdminNotes("Rejected");

    if (fundRequestRepository.save(*request))
    {
      transactionRepository.commitTransaction();
      return true;
    }
    else
    {
      transactionRepository.rollbackTransaction();
      return false;
    }
  }
}