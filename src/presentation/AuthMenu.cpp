#include "AuthMenu.h"
#include "ConsoleUtils.h"
#include<iostream>
#include<cstdlib>
#include<limits>

ActiveSession AuthMenu::handleAdminLogin()
{
    std::string username, password;

    std::cout << std::endl;
    std::cout << "=== ADMINISTRATOR LOGIN ===\n";
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::unique_ptr<Administrator> admin = authService.loginAdmin(username, password);
    if(admin != nullptr)
    {

        if(!admin->getIsActive())
        {
            std::cout << "❌ Error: This admin account is disabled.\n";
            return ActiveSession(); // Returns empty session (-1, -1)
        }

        std::cout << "✅ Login Successful! Welcome, " << admin->getFirstName() << ".\n";

        ActiveSession session;
        session.adminId = admin->getAdminId();
        return session;
    }

    else
    {
        std::cout << "❌ Invalid Username or Password.\n";
        return ActiveSession(); // Returns empty session (-1, -1)
    }


}

ActiveSession AuthMenu::handleUserLogin()
{
    std::string username, password;

    std::cout << std::endl;
    std::cout << "=== MEMBER LOGIN ===\n";
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::unique_ptr<User> user = authService.loginUser(username, password);
    if (user != nullptr)
    {
        if (!user->getIsActive())
        {
            std::cout << "❌ Error: This account is disabled or suspended. Please contact the administrator.\n";
            return ActiveSession(); // Returns empty session (-1, -1)
        }

        std::cout << "✅ Login Successful! Welcome, " << user->getFirstName() << ".\n";

        ActiveSession session;
        session.userId = user->getUserId(); 
        return session;
    }
    else
    {
        std::cout << "❌ Invalid Username or Password.\n";
        return ActiveSession(); // Returns empty session (-1, -1)
    }
}

ActiveSession AuthMenu::displayMenu()
{
    int choice = -1;
    bool running = true;
    ActiveSession currentSession; // Defaults to: userId = -1, adminId = -1, isExit = false

    while (running)
    {
        ConsoleUtils::clearScreen();

        // Print the Header
        std::cout << "========================================\n";
        std::cout << "      LIBRARY AND RESOURCE MANAGEMENT SYSTEM      \n";
        std::cout << "========================================\n";
        std::cout << "1. Member Login\n";
        std::cout << "2. Administrator Login\n";
        std::cout << "3. Register as New Member\n";
        std::cout << "0. Exit System\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        // Safe Input Capture
        if (!(std::cin >> choice))
        {
            // The user typed a character instead of an integer. Clear the error state!
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; // Silently restart the loop and redraw the menu
        }

        // State Machine Routing
        switch (choice)
        {
        case 1:
            currentSession = handleUserLogin();
            // If login was successful, break the loop and return to main.cpp
            if (currentSession.userId != -1)
                return currentSession;

            // If login failed, pause the screen so they can read the error
            std::cout << "Press Enter to return to main menu...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;

        case 2:
            currentSession = handleAdminLogin();
            if (currentSession.adminId != -1)
                return currentSession;

            std::cout << "Press Enter to return to main menu...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;

        case 3:
            handleRegistration();
            std::cout << "Press Enter to return to main menu...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;

        case 0:
            currentSession.isExit = true;
            return currentSession; // Send the exit signal back to main.cpp

        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }

    return currentSession; // Fallback return
}