#pragma once
#include <string>

std::string getCurrentDate();
std::string getDueDate(int daysToAdd = 14, std::string currentDate = getCurrentDate());