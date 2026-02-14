#ifndef PERSON_H
#define PERSON_H
#include <string>

class Person
{
protected:

    std::string username;
    std::string password;
    std::string first_name;
    std::string last_name;
    std::string email;
    bool is_active;

public:
    Person();
    Person(const std::string &uname, const std::string &pass,
           const std::string &fname, const std::string &lname,
           const std::string &mail, bool active = true);

    virtual ~Person();

    // Getters
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getEmail() const;
    bool getIsActive() const;

    // Setters
    void setUsername(const std::string &uname);
    void setPassword(const std::string &pass);
    void setFirstName(const std::string &fname);
    void setLastName(const std::string &lname);
    void setEmail(const std::string &mail);
    void setIsActive(bool active);
};

#endif // PERSON_H