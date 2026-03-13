#include "validator.h"
#include "../domain/Resource.h"
#include "../domain/User.h"

namespace Validator
{

    // Validates a Resource
    inline ValidationResult validate(const Resource &resource)
    {
        ValidationResult result;

        if (resource.getTitle().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: Title cannot be empty.");
        }

        if (resource.getAuthor().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: Author cannot be empty.");
        }

        if (resource.getPublisher().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: Publisher cannot be empty.");
        }
        if (resource.getPublicationYear() <= 0)
        {
            result.isValid = false;
            result.errors.push_back("Error: Publication year must be a positive integer.");
        }

         if (resource.getIsbn().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: ISBN cannot be empty.");
        }

         if (resource.getTotalCopies() < 0)
        {
            result.isValid = false;
            result.errors.push_back("Error: Total copies cannot be negative.");
        }

         if (resource.getAvailableCopies() < 0 || resource.getAvailableCopies() > resource.getTotalCopies())
        {
            result.isValid = false;
            result.errors.push_back("Error: Available copies must be between 0 and total copies.");
        }

        return result;
    }

    // Validates a User
    inline ValidationResult validate(const User &user)
    {
        ValidationResult result;

        if (user.getUsername().length() < 5)
        {
            result.isValid = false;
            result.errors.push_back("Error: Username must be at least 5 characters.");
        }
        if (user.getPassword().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: Password cannot be empty.");
        }
        if (user.getEmail().empty())
        {
            result.isValid = false;
            result.errors.push_back("Error: Email cannot be empty.");
        }
         if (user.getBalance() < 0)
        {
            result.isValid = false;
            result.errors.push_back("Error: Balance cannot be negative.");
        }

         if (user.getMembershipTypeId() <= 0)
        {
            result.isValid = false;
            result.errors.push_back("Error: Membership type ID must be a positive integer.");
        }

        return result;
    }
}