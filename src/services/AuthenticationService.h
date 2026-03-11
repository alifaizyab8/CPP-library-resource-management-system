#pragma once

#include <string>
#include <memory>
// Include the domain models 
#include "../domain/User.h"
#include "../domain/Administrator.h"

// Forward declarations
class UserRepository;
class AdministratorRepository;

class AuthenticationService {
    private:
        UserRepository &userRepository;
        AdministratorRepository &administratorRepository;

    public:
        AuthenticationService(UserRepository &userRepo, AdministratorRepository &adminRepo);

        // Authentication Methods
        std::unique_ptr<User> loginUser(const std::string &username, const std::string &password);
        std::unique_ptr<Administrator> loginAdmin(const std::string &username, const std::string &password);

        // Create new user/admin accounts (if needed)
        bool registerUser(User &user);
        bool registerAdmin(Administrator &admin);


};