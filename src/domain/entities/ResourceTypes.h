#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H
#include <string>

class ResourceType
{
private:
    int resource_type_id;
    std::string type_name;       // e.g., 'Book', 'DVD', 'Journal'
    int borrowing_duration_days; // DB Default: 14
    int max_renewals;            // DB Default: 2
    double fine_per_day;         // DB Default: 5.00
    std::string description;

public:
    // Constructors & Destructor
    ResourceType();

    ResourceType(const std::string &name, const std::string &desc,
                 int borrow_days = 14, int renewals = 2, double fine = 5.00);

    // For loading an EXISTING resource type from the database
    ResourceType(int type_id, const std::string &name, int borrow_days,
                 int renewals, double fine, const std::string &desc);

    ~ResourceType();

    // Getters
    int getResourceTypeId() const;
    std::string getTypeName() const;
    int getBorrowingDurationDays() const;
    int getMaxRenewals() const;
    double getFinePerDay() const;
    std::string getDescription() const;

    // Setters
    void setResourceTypeId(int type_id);
    void setTypeName(const std::string &name);
    void setBorrowingDurationDays(int borrow_days);
    void setMaxRenewals(int renewals);
    void setFinePerDay(double fine);
    void setDescription(const std::string &desc);
};

#endif // RESOURCETYPE_H