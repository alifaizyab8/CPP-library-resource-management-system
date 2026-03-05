#pragma once
#include <string>

class Transaction
{
private:
    int transactionId;
    int userId;
    int resourceId;
    std::string issueDate;
    std::string dueDate;
    std::string returnDate;
    double fineAmount;
    bool isReturned;
    bool isOverdue;
    int renewalCount;
    std::string transactionStatus;

public:
    Transaction() : transactionId(0), userId(0), resourceId(0), fineAmount(0.0),
                    isReturned(false), isOverdue(false), renewalCount(0) {}

    Transaction(int id, int uId, int rId, const std::string &iDate, const std::string &dDate,
                const std::string &rDate, double fine, bool returned, bool overdue, int renewals, const std::string &status)
        : transactionId(id), userId(uId), resourceId(rId), issueDate(iDate), dueDate(dDate),
          returnDate(rDate), fineAmount(fine), isReturned(returned), isOverdue(overdue), renewalCount(renewals), transactionStatus(status) {}

    // Getters
    int getTransactionId() const { return transactionId; }
    int getUserId() const { return userId; }
    int getResourceId() const { return resourceId; }
    std::string getIssueDate() const { return issueDate; }
    std::string getDueDate() const { return dueDate; }
    std::string getReturnDate() const { return returnDate; }
    double getFineAmount() const { return fineAmount; }
    bool getIsReturned() const { return isReturned; }
    bool getIsOverdue() const { return isOverdue; }
    int getRenewalCount() const { return renewalCount; }
    std::string getTransactionStatus() const { return transactionStatus; }

    // Setters
    void setTransactionId(int id) { transactionId = id; }
    void setUserId(int id) { userId = id; }
    void setResourceId(int id) { resourceId = id; }
    void setIssueDate(const std::string &d) { issueDate = d; }
    void setDueDate(const std::string &d) { dueDate = d; }
    void setReturnDate(const std::string &d) { returnDate = d; }
    void setFineAmount(double amount) { fineAmount = amount; }
    void setIsReturned(bool returned) { isReturned = returned; }
    void setIsOverdue(bool overdue) { isOverdue = overdue; }
    void setRenewalCount(int count) { renewalCount = count; }
    void setTransactionStatus(const std::string &s) { transactionStatus = s; }
};