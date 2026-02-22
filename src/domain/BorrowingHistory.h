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
    BorrowingHistory();
    BorrowingHistory(int userId, int resourceId, string issueDate,
                     string dueDate, string returnDate, double fine);

    int getId() const;
    int getUserId() const;
    int getResourceId() const;
    string getIssueDate() const;
    string getDueDate() const;
    string getReturnDate() const;
    double getFineAmount() const;

    void setId(int id);
};