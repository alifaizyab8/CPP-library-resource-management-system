#pragma once
#include <string>

class User
{
private:
    int userId;
    std::string username;
    std::string password;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string address;
    std::string phone;
    double balance;
    int membershipTypeId;
    std::string registrationDate;
    bool isActive;

public:
    User(int id, const std::string &uname, const std::string &pass, const std::string &fName,
         const std::string &lName, const std::string &email, const std::string &addr, const std::string &phone,
         double bal, int memTypeId, const std::string &regDate, bool active)
        : userId(id), username(uname), password(pass), firstName(fName), lastName(lName), email(email),
          address(addr), phone(phone), balance(bal), membershipTypeId(memTypeId), registrationDate(regDate), isActive(active) {}

    // Added Second Constrcutor for New Users
    User(const std::string &uname, const std::string &pass, const std::string &fName,
         const std::string &lName, const std::string &email, const std::string &addr, const std::string &phone,
         double bal, int memTypeId, const std::string &regDate, bool active)
        : userId(0), username(uname), password(pass), firstName(fName), lastName(lName), email(email),
          address(addr), phone(phone), balance(bal), membershipTypeId(memTypeId), registrationDate(regDate), isActive(active) {}
    int getUserId() const
    {
        return userId;
    }
    std::string getUsername() const
    {
        return username;
    }
    std::string getPassword() const
    {
        return password;
    }
    std::string getFirstName() const
    {
        return firstName;
    }
    std::string getLastName() const
    {
        return lastName;
    }
    std::string getEmail() const
    {
        return email;
    }
    std::string getAddress() const
    {
        return address;
    }
    std::string getPhone() const
    {
        return phone;
    }
    double getBalance() const
    {
        return balance;
    }
    int getMembershipTypeId() const
    {
        return membershipTypeId;
    }
    std::string getRegistrationDate() const
    {
        return registrationDate;
    }
    bool getIsActive() const
    {
        return isActive;
    }
    void setUserId(int id)
    {
        userId = id;
    }
    void setBalance(double newBalance)
    {
        balance = newBalance;
    }
};