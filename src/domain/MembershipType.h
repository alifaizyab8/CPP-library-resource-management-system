#ifndef MEMBERSHIPTYPE_H
#define MEMBERSHIPTYPE_H
#include <string>

class MembershipType
{
private:
    int membership_type_id;
    std::string membership_name;
    int duration_days;
    double price;
    int max_borrowing_limit;     // DB Default: 2
    int borrowing_duration_days; // DB Default: 14
    double fine_per_day;         // DB Default: 5.00
    std::string description;

public:
    // Constructors & Destructor
    MembershipType();

    MembershipType(const std::string &name, int duration, double prc,
                   const std::string &desc, int max_limit = 2,
                   int borrow_duration = 14, double fine = 5.00);

    // For loading an EXISTING membership type from the database
    MembershipType(int type_id, const std::string &name, int duration, double prc,
                   int max_limit, int borrow_duration, double fine,
                   const std::string &desc);

    ~MembershipType();

    // Getters
    int getMembershipTypeId() const;
    std::string getMembershipName() const;
    int getDurationDays() const;
    double getPrice() const;
    int getMaxBorrowingLimit() const;
    int getBorrowingDurationDays() const;
    double getFinePerDay() const;
    std::string getDescription() const;

    // Setters
    void setMembershipTypeId(int type_id);
    void setMembershipName(const std::string &name);
    void setDurationDays(int duration);
    void setPrice(double prc);
    void setMaxBorrowingLimit(int max_limit);
    void setBorrowingDurationDays(int borrow_duration);
    void setFinePerDay(double fine);
    void setDescription(const std::string &desc);
};

#endif // MEMBERSHIPTYPE_H
