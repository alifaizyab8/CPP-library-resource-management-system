#pragma once
#include <string>
#include "../services/UserService.h"

class UserMenu
{
private:
    UserService &userService;
    int currentUserId;
    std::string currentDate;

    // All possible menu actions
    void viewProfile();
    void updateProfile();
    void browseCatalogue();
    void searchCatalogue();
    void requestToBorrow();
    void viewActiveResources();
    void cancelPendingRequest();
    void viewTransactionHistory();
    void viewCurrentFines();
    void requestBalanceTopUp();
    void requestAccountDeletion();

    // helper
    void pauseAndClear();

public:
    UserMenu(UserService &uService, int userId, const std::string &simulatedDate);

    // main loop
    void handleUserUI();
};