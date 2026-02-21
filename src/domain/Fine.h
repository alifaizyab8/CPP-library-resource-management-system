#pragma once
#include <string>

class Fine
{
private:
    int fineId;
    int transactionId;
    int userId;
    int daysOverdue;
    double fineAmount;
    std::string fineDate;
    bool isPaid;
    std::string paymentDate;

public:
    Fine() : fineId(0), transactionId(0), userId(0), daysOverdue(0), fineAmount(0.0), isPaid(false) {}

    Fine(int id, int tId, int uId, int days, double amount, const std::string &fDate, bool paid, const std::string &pDate)
        : fineId(id), transactionId(tId), userId(uId), daysOverdue(days), fineAmount(amount), fineDate(fDate), isPaid(paid), paymentDate(pDate) {}

    // Getters
    int getFineId() const { return fineId; }
    int getTransactionId() const { return transactionId; }
    int getUserId() const { return userId; }
    int getDaysOverdue() const { return daysOverdue; }
    double getFineAmount() const { return fineAmount; }
    std::string getFineDate() const { return fineDate; }
    bool getIsPaid() const { return isPaid; }
    std::string getPaymentDate() const { return paymentDate; }

    // Setters
    void setFineId(int id) { fineId = id; }
    void setTransactionId(int tId) { transactionId = tId; }
    void setUserId(int uId) { userId = uId; }
    void setDaysOverdue(int days) { daysOverdue = days; }
    void setFineAmount(double amount) { fineAmount = amount; }
    void setFineDate(const std::string &fDate) { fineDate = fDate; }
    void setIsPaid(bool paid) { isPaid = paid; }
    void setPaymentDate(const std::string &pDate) { paymentDate = pDate; }
};