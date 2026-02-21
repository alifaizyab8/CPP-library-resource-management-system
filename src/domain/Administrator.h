#pragma once
#include <string>

class Administrator
{
private:
    int adminId;
    std::string username;
    std::string password;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string createdDate;
    bool isActive;

public:
    Administrator(int id, const std::string &uname, const std::string &pass, const std::string &fName,
                  const std::string &lName, const std::string &email, const std::string &cDate, bool active)
        : adminId(id), username(uname), password(pass), firstName(fName), lastName(lName),
          email(email), createdDate(cDate), isActive(active) {}

    int getAdminId() const { return adminId; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getEmail() const { return email; }
    std::string getCreatedDate() const { return createdDate; }
    bool getIsActive() const { return isActive; }
    void setAdminId(int id);
};