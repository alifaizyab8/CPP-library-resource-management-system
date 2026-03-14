#pragma once

#include <string>
#include <vector>


#include "../domain/Resource.h"
#include "../domain/User.h"

namespace Validator
{
    // 1. The struct is defined INSIDE the namespace
    struct ValidationResult
    {
        bool isValid;
        std::vector<std::string> errors; // Holds all validation errors

        // Helper constructor to default to "Valid"
        ValidationResult() : isValid(true) {}
    };

    // 2. Function Declarations 
    ValidationResult validate(const Resource &resource);
    ValidationResult validate(const User &user);
}