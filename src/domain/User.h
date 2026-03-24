#pragma once
#include "Person.h"
#include <string>

class User : public Person
{
private:
    std::string address;
    std::string phone;
    double balance;
    int membershipTypeId;
    std::string registrationDate;
    bool deletionRequested;

public:
    // Default Constructor
    User() : Person(), balance(0.0), membershipTypeId(0), deletionRequested(false) {}

    // Parameterized Constructor
    User(int id, const std::string &uname, const std::string &pass, const std::string &fName,
         const std::string &lName, const std::string &email, const std::string &addr, const std::string &phone,
         double bal, int memTypeId, const std::string &regDate, bool active, bool delRequest = false)

        : Person(id, uname, pass, fName, lName, email, active),
          address(addr), phone(phone), balance(bal), membershipTypeId(memTypeId), registrationDate(regDate), deletionRequested(delRequest)
    {
    }

    // Overriding Pure Virtual Function (Satisfies Polymorphism)
    std::string getRole() const override { return "Member"; }

    int getUserId() const { return id; }
    void setUserId(int userId) { id = userId; }

    // Unique Getters
    std::string getAddress() const { return address; }
    std::string getPhone() const { return phone; }
    double getBalance() const { return balance; }
    int getMembershipTypeId() const { return membershipTypeId; }
    std::string getRegistrationDate() const { return registrationDate; }
    bool getDeletionRequested() const { return deletionRequested; }

    // Unique Setters
    void setAddress(const std::string &addr) { address = addr; }
    void setPhone(const std::string &p) { phone = p; }
    void setBalance(double bal) { balance = bal; }
    void setMembershipTypeId(int id) { membershipTypeId = id; }
    void setRegistrationDate(const std::string &date) { registrationDate = date; }
    void setDeletionRequested(bool req) { deletionRequested = req; }
};