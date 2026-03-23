#include "date.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <limits>

// Helper function to safely read an integer and handle bad inputs (like letters)
int getValidIntInput(int min, int max, const std::string &prompt)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max)
        {
            return value;
        }
        else
        {
            std::cout << "[!] Invalid input. Please enter a number between " << min << " and " << max << ".\n";
            std::cin.clear();                                                   // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        }
    }
}

// Helper to format date with leading zeros (YYYY-MM-DD)
std::string formatDate(int year, int month, int day)
{
    std::ostringstream oss;
    oss << year << "-"
        << std::setw(2) << std::setfill('0') << month << "-"
        << std::setw(2) << std::setfill('0') << day;
    return oss.str();
}

std::string getCurrentDate()
{
    std::cout << "\n--- System Date Configuration ---\n";
    int year = getValidIntInput(2000, 2100, "Enter current year (e.g., 2023): ");
    int month = getValidIntInput(1, 12, "Enter current month (1-12): ");

    // Determine max days based on the month (and leap year for Feb)
    int maxDays = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        maxDays = 30;
    }
    else if (month == 2)
    {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        maxDays = isLeapYear ? 29 : 28;
    }

    int day = getValidIntInput(1, maxDays, "Enter current day (1-" + std::to_string(maxDays) + "): ");

    return formatDate(year, month, day);
}

std::string getDueDate(int daysToAdd, std::string currentDate)
{
    std::tm tm = {};
    std::istringstream ss(currentDate);

    // Parse the strict YYYY-MM-DD format
    ss >> std::get_time(&tm, "%Y-%m-%d");

    // Let C++ handle the complex math!
    tm.tm_mday += daysToAdd;

    // std::mktime automatically normalizes the date.
    // Example: If adding 14 days makes it Oct 35th, mktime converts it to Nov 4th!
    std::mktime(&tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

int calculateDaysOverdue(const std::string &dueDateStr, const std::string &todayStr)
{
    std::tm dueTm = {};
    std::tm todayTm = {};

    std::istringstream ssDue(dueDateStr);
    std::istringstream ssToday(todayStr);

    ssDue >> std::get_time(&dueTm, "%Y-%m-%d");
    ssToday >> std::get_time(&todayTm, "%Y-%m-%d");

    // Force time to midnight and ignore Daylight Saving Time to prevent 1-day math errors
    dueTm.tm_hour = 0;
    dueTm.tm_min = 0;
    dueTm.tm_sec = 0;
    dueTm.tm_isdst = -1;
    todayTm.tm_hour = 0;
    todayTm.tm_min = 0;
    todayTm.tm_sec = 0;
    todayTm.tm_isdst = -1;

    std::time_t dueTime = std::mktime(&dueTm);
    std::time_t todayTime = std::mktime(&todayTm);

    if (dueTime == -1 || todayTime == -1)
    {
        return 0; // Return 0 if parsing fails so we don't crash
    }

    double difference = std::difftime(todayTime, dueTime);
    int daysOverdue = difference / (60 * 60 * 24); // Convert seconds to days

    // If the book is returned early, difference is negative. We don't owe fines for early returns!
    return (daysOverdue > 0) ? daysOverdue : 0;
}