#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H
#include "../additional_classes/Person.h" // Include the base class header
#include <string>

class Administrator : public Person
{
    private:
        int admin_id;
        std::string created_date;

    public:
        Administrator();

        Administrator(int id, const std::string &uname, const std::string &pass,
                    const std::string &fname, const std::string &lname,
                    const std::string &mail, const std::string &date, bool active = true);

        ~Administrator();

        // Getters 
        int getAdminId() const;
        std::string getCreatedDate() const;

        // Setters 
        void setAdminId(int id);
        void setCreatedDate(const std::string &date);
};

#endif // ADMINISTRATOR_H
