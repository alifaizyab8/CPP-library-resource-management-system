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
    User() : userId(0), balance(0.0), membershipTypeId(0), isActive(false) {}

    User(int id, const std::string &uname, const std::string &pass, const std::string &fName,
         const std::string &lName, const std::string &email, const std::string &addr, const std::string &phone,
         double bal, int memTypeId, const std::string &regDate, bool active)
        : userId(id), username(uname), password(pass), firstName(fName), lastName(lName), email(email),
          address(addr), phone(phone), balance(bal), membershipTypeId(memTypeId), registrationDate(regDate), isActive(active) {}

    // Getters
    int getUserId() const { return userId; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getEmail() const { return email; }
    std::string getAddress() const { return address; }
    std::string getPhone() const { return phone; }
    double getBalance() const { return balance; }
    int getMembershipTypeId() const { return membershipTypeId; }
    std::string getRegistrationDate() const { return registrationDate; }
    bool getIsActive() const { return isActive; }

    // Setters
    void setUserId(int id) { userId = id; }
    void setUsername(const std::string &uname) { username = uname; }
    void setPassword(const std::string &pass) { password = pass; }
    void setFirstName(const std::string &fName) { firstName = fName; }
    void setLastName(const std::string &lName) { lastName = lName; }
    void setEmail(const std::string &e) { email = e; }
    void setAddress(const std::string &addr) { address = addr; }
    void setPhone(const std::string &p) { phone = p; }
    void setBalance(double bal) { balance = bal; }
    void setMembershipTypeId(int id) { membershipTypeId = id; }
    void setRegistrationDate(const std::string &date) { registrationDate = date; }
    void setIsActive(bool active) { isActive = active; }
};