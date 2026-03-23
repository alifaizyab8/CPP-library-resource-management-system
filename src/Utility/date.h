#pragma once
#include <string>

// --- Core Date Functions ---
std::string getCurrentDate();
std::string getDueDate(int daysToAdd = 14, std::string currentDate = getCurrentDate());
int calculateDaysOverdue(const std::string &dueDateStr, const std::string &todayStr);

// --- Utility Helper Functions ---
// Safely gets an integer from the user and prevents infinite loop crashes
int getValidIntInput(int min, int max, const std::string &prompt);

// Formats integers into a strict "YYYY-MM-DD" string with leading zeros
std::string formatDate(int year, int month, int day);