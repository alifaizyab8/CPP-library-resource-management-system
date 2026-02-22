#pragma once
#include <string>
using namespace std;

class BorrowingHistory
{
private:
    int historyId;
    int userId;
    int resourceId;
    string issueDate;
    string dueDate;
    string returnDate;
    double fineAmount;

public:
    // Constructors
    BorrowingHistory()
        : historyId(0), userId(0), resourceId(0), issueDate(""),
          dueDate(""), returnDate(""), fineAmount(0.0) {}

    BorrowingHistory(int userId, int resourceId, string issueDate,
                     string dueDate, string returnDate, double fine)
        : historyId(0), userId(userId), resourceId(resourceId),
          issueDate(issueDate), dueDate(dueDate), returnDate(returnDate),
          fineAmount(fine) {}

    // Getters
    int getId() const { return historyId; }
    int getUserId() const { return userId; }
    int getResourceId() const { return resourceId; }
    string getIssueDate() const { return issueDate; }
    string getDueDate() const { return dueDate; }
    string getReturnDate() const { return returnDate; }
    double getFineAmount() const { return fineAmount; }

    // Setter
    void setId(int id) { historyId = id; }
};

