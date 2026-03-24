#pragma once
#include "Person.h"
#include <string>

class Administrator : public Person
{
private:
    std::string createdDate;

public:
    // Default Constructor
    Administrator() : Person(), createdDate("")
    {
        this->isActive = true;
    }

    // Parameterized Constructor
    Administrator(int id, const std::string &uname, const std::string &pass, const std::string &fName,
                  const std::string &lName, const std::string &email, const std::string &cDate, bool active)
        : Person(id, uname, pass, fName, lName, email, active), createdDate(cDate) {}

    // Overriding Pure Virtual Function (Satisfies Polymorphism)
    std::string getRole() const override { return "Administrator"; }

    int getAdminId() const { return id; }
    void setAdminId(int adminId) { id = adminId; }

    // Unique Getters/Setters
    std::string getCreatedDate() const { return createdDate; }
    void setCreatedDate(const std::string &cDate) { createdDate = cDate; }
};