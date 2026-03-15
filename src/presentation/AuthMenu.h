#pragma once

#include "Session.h"
#include "../services/AuthenticationService.h"
#include <string>

class AuthMenu
{
    private:
        AuthenticationService &authService;
        std::string dateToday;

        ActiveSession handleAdminLogin();
        ActiveSession handleUserLogin();
        //void handleRegistration();

    public:
        AuthMenu(AuthenticationService &authSvc, const std::string &today);

        ActiveSession displayMenu();
};