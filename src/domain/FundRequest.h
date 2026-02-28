#pragma once
#include <string>

class FundRequest
{
private:
    int requestId;
    int userId;
    double requestedAmount;
    std::string requestDate;
    std::string status;
    int adminId;
    std::string approvalDate;
    std::string adminNotes;

public:
    FundRequest() : requestId(0), userId(0), requestedAmount(0.0), adminId(0) {}

    FundRequest(int id, int uId, double amount, const std::string &rDate, const std::string &stat,
                int aId, const std::string &aDate, const std::string &notes)
        : requestId(id), userId(uId), requestedAmount(amount), requestDate(rDate), status(stat),
          adminId(aId), approvalDate(aDate), adminNotes(notes) {}

    // Getters
    int getRequestId() const { return requestId; }
    int getUserId() const { return userId; }
    double getRequestedAmount() const { return requestedAmount; }
    std::string getRequestDate() const { return requestDate; }
    std::string getStatus() const { return status; }
    int getAdminId() const { return adminId; }
    std::string getApprovalDate() const { return approvalDate; }
    std::string getAdminNotes() const { return adminNotes; }

    // Setters
    void setRequestId(int id) { requestId = id; }
    void setUserId(int uId) { userId = uId; }
    void setRequestedAmount(double amount) { requestedAmount = amount; }
    void setRequestDate(const std::string &rDate) { requestDate = rDate; }
    void setStatus(const std::string &stat) { status = stat; }
    void setAdminId(int aId) { adminId = aId; }
    void setApprovalDate(const std::string &aDate) { approvalDate = aDate; }
    void setAdminNotes(const std::string &notes) { adminNotes = notes; }
};
