#include"date.h"
#include <iostream>
std::string getCurrentDate()
{
    std::cout << "Enter current year: ";
    int year;
    std::cin >> year;
    std::cout << "Enter current month (1-12): ";
    int month;
    std::cin >> month;
    std::cout << "Enter current day (1-31): ";
    int day;
    std::cin >> day;

    return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
}

std::string getDueDate(int daysToAdd, std::string currentDate=getCurrentDate())
{
    // parse currentDate
    int year, month, day;
    sscanf(currentDate.c_str(), "%d-%d-%d", &year, &month, &day);

    // Simple logic to add days 
    day += daysToAdd;

    // Handle simple overflow of days 
    if (day > 30)
    {
        day -= 30;
        month += 1;
        if (month > 12)
        {
            month = 1;
            year += 1;
        }
    }

    return std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
}

#include <ctime>
#include <iomanip>
#include <sstream>

// Helper to calculate difference in days
int calculateDaysOverdue(const std::string &dueDateStr, const std::string &todayStr)
{
    std::tm dueTm = {};
    std::tm todayTm = {};

    std::istringstream ssDue(dueDateStr);
    std::istringstream ssToday(todayStr);

    ssDue >> std::get_time(&dueTm, "%Y-%m-%d");
    ssToday >> std::get_time(&todayTm, "%Y-%m-%d");

    std::time_t dueTime = std::mktime(&dueTm);
    std::time_t todayTime = std::mktime(&todayTm);

    if (dueTime == -1 || todayTime == -1)
        return 0; // Error parsing

    double difference = std::difftime(todayTime, dueTime);
    return difference / (60 * 60 * 24); // Convert seconds to days
}