#include "AuthenticationService.h"

#include "../infrastructure/repositories/UserRepository.h"
#include "../infrastructure/repositories/AdministratorRepository.h"

// Constructor
AuthenticationService::AuthenticationService(UserRepository &userRepo, AdministratorRepository &adminRepo)
    : userRepository(userRepo), administratorRepository(adminRepo) {}


    //Login Methods
std::unique_ptr<Administrator> AuthenticationService::loginAdmin(const std::string &username, const std::string &password)
{

    std::unique_ptr<Administrator> admin = administratorRepository.getByUsername(username);

    if(admin !=nullptr && admin->getPassword()==password){
        return admin;
    } 

    return nullptr;
}


std::unique_ptr<User> AuthenticationService::loginUser(const std::string &username, const std::string &password)
{

    std::unique_ptr<User> user = userRepository.getByUsername(username);

    if(user !=nullptr && user->getPassword()==password){
        return user;
    }

    return nullptr;
}


// Registration Methods
bool AuthenticationService::registerUser(User &user)
{
    if(userRepository.getByUsername(user.getUsername())!= nullptr){
        return false; //already exists
}

    return userRepository.save(user);

}


bool AuthenticationService::registerAdmin(Administrator &admin)
{
    if(administratorRepository.getByUsername(admin.getUsername())!= nullptr){
        return false; //already exists
    }

    return administratorRepository.save(admin);
}