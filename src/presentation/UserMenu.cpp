#include "UserMenu.h"
#include "ConsoleUtils.h"
#include "../validation/validator.h"
#include <iostream>
#include <iomanip>
#include <limits>

UserMenu::UserMenu(UserService &uService, int userId, const std::string &simulatedDate)
    : userService(uService), currentUserId(userId), currentDate(simulatedDate) {}

void UserMenu::pauseAndClear()
{
    std::cout << "\nPress Enter to return to the menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void UserMenu::handleUserUI()
{
    bool running = true;
    int choice = -1;

    while (running)
    {
        ConsoleUtils::clearScreen();

        std::unique_ptr<User> user = userService.getUserDetails(currentUserId);
        std::string greetingName = (user != nullptr) ? user->getFirstName() : "Member"; // if no name then default to user

        std::cout << "========================================\n";
        std::cout << "          MEMBER DASHBOARD              \n";
        std::cout << " Welcome, " << greetingName << " | Date: " << currentDate << "\n";
        std::cout << "========================================\n";
        std::cout << " --- Profile & Account ---\n";
        std::cout << " 1. View My Profile\n";
        std::cout << " 2. Update Profile Details\n";
        std::cout << " 3. View Current Fines\n";
        std::cout << " 4. Top-up Account Balance\n";
        std::cout << "\n --- Library Catalogue ---\n";
        std::cout << " 5. Browse Available Resources\n";
        std::cout << " 6. Search Resources\n";
        std::cout << " 7. Request to Borrow a Resource\n";
        std::cout << "\n --- My Library & History ---\n";
        std::cout << " 8. View Active & Pending Resources\n";
        std::cout << " 9. Cancel a Pending Borrow Request\n";
        std::cout << " 10. View Transaction & Borrowing History\n";
        std::cout << "\n --- System ---\n";
        std::cout << " 11. Request Account Deletion\n";
        std::cout << " 0. Logout\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            viewProfile();
            break;
        case 2:
            updateProfile();
            break;
        case 3:
            viewCurrentFines();
            break;
        case 4:
            requestBalanceTopUp();
            break;
        case 5:
            browseCatalogue();
            break;
        case 6:
            searchCatalogue();
            break;
        case 7:
            requestToBorrow();
            break;
        case 8:
            viewActiveResources();
            break;
        case 9:
            cancelPendingRequest();
            break;
        case 10:
            viewTransactionHistory();
            break;
        case 11:
            requestAccountDeletion();
            break;
        case 0:
            std::cout << "Logging out...\n";
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            pauseAndClear();
            break;
        }
    }
}

void UserMenu::viewProfile()
{
    std::unique_ptr<User> user = userService.getUserDetails(currentUserId);
    if (user != nullptr)
    {
        std::cout << "\n=== MY PROFILE ===\n";
        std::cout << "Name:       " << user->getFirstName() << " " << user->getLastName() << "\n";
        std::cout << "Username:   " << user->getUsername() << "\n";
        std::cout << "Email:      " << user->getEmail() << "\n";
        std::cout << "Phone:      " << user->getPhone() << "\n";
        std::cout << "Address:    " << user->getAddress() << "\n";
        std::cout << "Balance:    $" << std::fixed << std::setprecision(2) << user->getBalance() << "\n";
        std::cout << "Reg Date:   " << user->getRegistrationDate() << "\n";
    }
    else
    {
        std::cout << "\nError: Could not retrieve user profile.\n";
    }
    pauseAndClear();
}

void UserMenu::updateProfile()
{
    std::unique_ptr<User> user = userService.getUserDetails(currentUserId);
    if (user == nullptr)
        return;

    std::cout << "\n=== UPDATE PROFILE ===\n";
    std::cout << "(Press Enter to keep current value)\n";

    std::string input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear buffer

    std::cout << "First Name [" << user->getFirstName() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        user->setFirstName(input);

    std::cout << "Last Name [" << user->getLastName() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        user->setLastName(input);

    std::cout << "Email [" << user->getEmail() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        user->setEmail(input);

    std::cout << "Phone [" << user->getPhone() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        user->setPhone(input);

    std::cout << "Address [" << user->getAddress() << "]: ";
    std::getline(std::cin, input);
    if (!input.empty())
        user->setAddress(input);

    // Validate input
    Validator::ValidationResult result = Validator::validate(*user);
    if (!result.isValid)
    {
        std::cout << "\n Update Failed due to invalid data:\n";
        for (const auto &err : result.errors)
        {
            std::cout << " - " << err << "\n";
        }
    }
    else
    {
        if (userService.updateProfile(*user))
            std::cout << "\nProfile updated successfully!\n";
        else
            std::cout << "\nError updating profile in the database.\n";
    }
    pauseAndClear();
}

void UserMenu::viewCurrentFines()
{
    std::vector<Fine> fines = userService.getCurrentFines(currentUserId);
    std::cout << "\n=== UNPAID FINES ===\n";
    if (fines.empty())
    {
        std::cout << "You have no unpaid fines! Great job.\n";
    }
    else
    {
        for (const auto &fine : fines)
        {
            std::cout << "Fine ID: " << fine.getFineId() << " | Trans ID: " << fine.getTransactionId() << " | Overdue: " << fine.getDaysOverdue() << " days" << " | Amount: $" << std::fixed << std::setprecision(2) << fine.getFineAmount() << "\n";
        }
    }
    pauseAndClear();
}

void UserMenu::requestBalanceTopUp()
{
    std::cout << "\n=== TOP-UP BALANCE ===\n";
    std::unique_ptr<User> user = userService.getUserDetails(currentUserId);
    std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << user->getBalance() << "\n";

    double amount;
    std::cout << "Enter amount to request: $";
    if (!(std::cin >> amount))
    {
        std::cout << "Invalid amount entered.\n";
        std::cin.clear();
        return pauseAndClear();
    }

    if (userService.requestFund(currentUserId, amount, currentDate))
    {
        std::cout << "Top-up request for $" << amount << " submitted! Waiting for Admin approval.\n";
    }
    else
    {
        std::cout << "Failed to submit request. Please enter a valid positive amount.\n";
    }
    pauseAndClear();
}

void UserMenu::browseCatalogue()
{
    std::vector<Resource> resources = userService.showAllAvailableCatalogue();
    std::cout << "\n=== AVAILABLE RESOURCES ===\n";
    if (resources.empty())
    {
        std::cout << "No resources are currently available.\n";
    }
    else
    {
        for (const auto &res : resources)
        {
            std::cout << "ID: " << res.getResourceId() << " | Title: " << res.getTitle() << " | Author: " << res.getAuthor() << " | Available: " << res.getAvailableCopies() << "\n";
        }
    }
    pauseAndClear();
}

void UserMenu::searchCatalogue()
{
    std::string keyword;
    std::cout << "\n=== SEARCH CATALOGUE ===\n";
    std::cout << "Enter title or author keyword: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, keyword);

    std::vector<Resource> matches = userService.searchCatalogue(keyword);
    std::cout << "\n--- Search Results ---\n";
    if (matches.empty())
    {
        std::cout << "No matching resources found.\n";
    }
    else
    {
        for (const auto &book : matches)
        {
            std::cout << "ID: " << book.getResourceId() << " | Title: " << book.getTitle()
                      << " | Author: " << book.getAuthor()
                      << " | Available: " << book.getAvailableCopies() << "\n";
        }
    }
    pauseAndClear();
}

void UserMenu::requestToBorrow()
{
    int resourceId;
    std::cout << "\n=== REQUEST TO BORROW ===\n";
    std::cout << "Enter the Resource ID you wish to borrow: ";
    if (!(std::cin >> resourceId))
    {
        std::cout << "Invalid input.\n";
        std::cin.clear();
        return pauseAndClear();
    }

    std::string result = userService.requestToBorrow(currentUserId, resourceId);
    std::cout << "\nSystem Response: " << result << "\n";
    pauseAndClear();
}

void UserMenu::viewActiveResources()
{
    std::cout << "\n=== MY ACTIVE RESOURCES ===\n";

    std::vector<Transaction> issued = userService.getCurrentlyBorrowedResources(currentUserId);
    std::cout << "\n--- Currently Issued (In your possession) ---\n";
    if (issued.empty())
        std::cout << "None.\n";
    else
    {
        for (const auto &txn : issued)
        {
            std::cout << "Trans ID: " << txn.getTransactionId() << " | Resource ID: " << txn.getResourceId() << " | Due Date: " << txn.getDueDate() << "\n";
        }
    }

    std::vector<Transaction> pending = userService.getPendingBorrowRequests(currentUserId);
    std::cout << "\n--- Pending Borrow Requests (Awaiting Admin) ---\n";
    if (pending.empty())
        std::cout << "None.\n";
    else
    {
        for (const auto &txn : pending)
        {
            std::cout << "Trans ID: " << txn.getTransactionId() << " | Resource ID: " << txn.getResourceId() << "\n";
        }
    }
    pauseAndClear();
}

void UserMenu::cancelPendingRequest()
{
    int transId;
    std::cout << "\n=== CANCEL PENDING REQUEST ===\n";

    // preview of the request which is about to be cancelled by the user
    std::vector<Transaction> pending = userService.getPendingBorrowRequests(currentUserId);
    if (pending.empty())
    {
        std::cout << "You have no pending requests to cancel.\n";
        return pauseAndClear();
    }

    std::cout << "Your pending Transaction IDs: ";
    for (const auto &txn : pending)
        std::cout << txn.getTransactionId() << " ";
    std::cout << "\n\nEnter Transaction ID to cancel: ";

    if (!(std::cin >> transId))
    {
        std::cout << "Invalid input.\n";
        std::cin.clear();
        return pauseAndClear();
    }

    if (userService.cancelPendingBorrowRequest(transId, currentUserId))
    {
        std::cout << "Request cancelled successfully.\n";
    }
    else
    {
        std::cout << "Failed to cancel. Make sure the ID is correct and still pending.\n";
    }
    pauseAndClear();
}

void UserMenu::viewTransactionHistory()
{
    std::cout << "\n=== FULL TRANSACTION HISTORY ===\n";
    std::vector<Transaction> txns = userService.getTransactionHistory(currentUserId);
    if (txns.empty())
    {
        std::cout << "No transactions found.\n";
    }
    else
    {
        for (const auto &txn : txns)
        {
            std::cout << "ID: " << txn.getTransactionId() << " | Resource: " << txn.getResourceId() << " | Status: " << txn.getTransactionStatus() << " | Fine: $" << txn.getFineAmount() << "\n";
        }
    }
    pauseAndClear();
}

void UserMenu::requestAccountDeletion()
{
    std::cout << "\n=== REQUEST ACCOUNT DELETION ===\n";
    std::cout << "Are you sure you want to permanently delete your account? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        std::string result = userService.requestAccountDeletion(currentUserId);
        std::cout << "\nSystem Response: " << result << "\n";
    }
    else
    {
        std::cout << "Deletion request aborted.\n";
    }
    pauseAndClear();
}