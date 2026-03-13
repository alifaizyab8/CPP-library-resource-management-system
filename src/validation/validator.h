#pragma once
#include <string>
#include <vector>

struct ValidationResult
{
    bool isValid;
    std::vector<std::string> errors; // Holds all the specific error messages

    // Helper constructor to default to "Valid"
    ValidationResult() : isValid(true) {}
};