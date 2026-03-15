#include "presentation/AdminMenu.h"
#include "presentation/ConsoleUtils.h"
#include "services/AdminService.h"
#include "../validation/validator.h"
#include <iostream>
#include <limits>

AdminMenu::AdminMenu(AdminService &service, const std::string &today)
    : adminService(service), simulatedToday(today), currentAdminId(-1) {}

/* *************************************************************************
                    ---------- MAIN DASHBOARD ----------
   ************************************************************************* */

void AdminMenu::displayDashboard(int adminId)
{
    currentAdminId = adminId;
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();

        std::cout << "========================================\n";
        std::cout << "          ADMINISTRATOR PORTAL          \n";
        std::cout << "          Date: " << simulatedToday << "\n";
        std::cout << "========================================\n";
        std::cout << "1.  Catalog Management\n";
        std::cout << "2.  Member Management\n";
        std::cout << "3.  Circulation Desk\n";
        std::cout << "4.  Financial Desk\n";
        std::cout << "5.  Reporting Engine\n";
        std::cout << "6.  System & Admin Settings\n";
        std::cout << "0.  Logout\n";
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
            displayCatalogMenu();
            break;
        case 2:
            displayMemberMenu();
            break;
        case 3:
            displayCirculationMenu();
            break;
        case 4:
            displayFinancialMenu();
            break;
        case 5:
            displayReportingMenu();
            break;
        case 6:
            displaySystemMenu();
            break;
        case 0:
            running = false;
            std::cout << "Logging out of Admin Portal...\n";
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

/* *************************************************************************
                 ---------- CATALOG MANAGEMENT ----------
   ************************************************************************* */

void AdminMenu::displayCatalogMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "           CATALOG MANAGEMENT           \n";
        std::cout << "========================================\n";
        std::cout << "1. Add New Resource\n";
        std::cout << "2. Edit Existing Resource\n";
        std::cout << "3. Delete Resource\n";
        std::cout << "4. View All Resources\n";
        std::cout << "5. Add New Category\n";
        std::cout << "6. Edit Category\n";
        std::cout << "7. Delete Category\n";
        std::cout << "8. View All Categories\n";
        std::cout << "0. Back to Main Dashboard\n";
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
            handleAddResource();
            break;
        case 2:
            handleEditResource();
            break;
        case 3:
            handleDeleteResource();
            break;
        case 4:
            handleViewAllResources();
            break;
        case 5:
            handleAddCategory();
            break;
        case 6:
            handleEditCategory();
            break;
        case 7:
            handleDeleteCategory();
            break;
        case 8:
            handleViewAllCategories();
            break;

        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleAddResource()
{
    std::string title, author, publisher, isbn;
    int categoryId, publicationYear, totalCopies;

    std::cout << "\n--- ADD NEW RESOURCE ---\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    std::cout << "Enter Title: ";
    std::getline(std::cin, title);

    std::cout << "Enter Author: ";
    std::getline(std::cin, author);

    std::cout << "Enter Publisher: ";
    std::getline(std::cin, publisher);

    std::cout << "Enter ISBN: ";
    std::getline(std::cin, isbn);

    std::cout << "Enter Publication Year: ";
    if (!(std::cin >> publicationYear))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "Enter Category ID: ";
    if (!(std::cin >> categoryId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "Enter Total Copies: ";
    if (!(std::cin >> totalCopies))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    Resource newResource;
    newResource.setTitle(title);
    newResource.setAuthor(author);
    newResource.setPublisher(publisher);
    newResource.setIsbn(isbn);
    newResource.setPublicationYear(publicationYear);
    newResource.setCategoryId(categoryId);
    newResource.setTotalCopies(totalCopies);
    newResource.setAvailableCopies(totalCopies); // Initially, all copies are available
    newResource.setIsActive(true);

    Validator::ValidationResult result = Validator::validate(newResource);

    if (!result.isValid)
    {
        std::cout << "\n Validation Failed:\n";
        for (const std::string &error : result.errors)
        {
            std::cout << "  - " << error << "\n";
        }
    }
    else
    {
        if (adminService.addResource(newResource))
        {
            std::cout << "\n Resource added successfully!\n";
        }
        else
        {
            std::cout << "\n Database Error: Could not add resource.\n";
        }
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleEditResource()
{
    int resourceId;
    std::cout << "\n--- EDIT RESOURCE ---\n";
    std::cout << "Enter Resource ID to Edit: ";

    if (!(std::cin >> resourceId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::unique_ptr<Resource> resource = adminService.getResourceById(resourceId);

    if (!resource)
    {
        std::cout << " Error: Resource ID " << resourceId << " not found.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }

    std::string tempStr;
    int tempInt;

    std::cout << "\n--- Current Resource Details ---\n";
    std::cout << "Title: " << resource->getTitle() << "\n";
    std::cout << "Author: " << resource->getAuthor() << "\n";
    std::cout << "Total Copies: " << resource->getTotalCopies() << "\n";
    std::cout << "----------------------------\n";
    std::cout << "Enter new values (or type '.' for strings, or 0 for numbers to keep the current value)\n\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer for getline

    std::cout << "New Title: ";
    std::getline(std::cin, tempStr);
    if (tempStr != ".")
        resource->setTitle(tempStr);

    std::cout << "New Author: ";
    std::getline(std::cin, tempStr);
    if (tempStr != ".")
        resource->setAuthor(tempStr);

    std::cout << "New Total Copies (current: " << resource->getTotalCopies() << "): ";
    if (std::cin >> tempInt && tempInt != 0)
    {
        // Adjust available copies based on the change in total copies
        int difference = tempInt - resource->getTotalCopies();
        resource->setTotalCopies(tempInt);
        resource->setAvailableCopies(resource->getAvailableCopies() + difference);
    }

    Validator::ValidationResult result = Validator::validate(*resource);

    if (!result.isValid)
    {
        std::cout << "\n Validation Failed:\n";
        for (const std::string &error : result.errors)
        {
            std::cout << "  - " << error << "\n";
        }
    }
    else
    {
        if (adminService.editResource(*resource))
        {
            std::cout << "\n Resource updated successfully!\n";
        }
        else
        {
            std::cout << "\n Database Error: Could not update resource.\n";
        }
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleDeleteResource()
{
    int resourceId;
    std::cout << "\n--- DELETE RESOURCE ---\n";
    std::cout << "Enter Resource ID to delete: ";

    if (!(std::cin >> resourceId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << " WARNING: Are you sure you want to delete this resource? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        if (adminService.deleteResource(resourceId))
        {
            std::cout << " Resource deleted successfully!\n";
        }
        else
        {
            std::cout << " Failed to delete resource. It may have active borrowings or reservations.\n";
        }
    }
    else
    {
        std::cout << "Deletion cancelled.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllResources()
{
    std::cout << "\n--- ALL RESOURCES ---\n";
    std::vector<Resource> resources = adminService.viewAllResources();
    if (resources.empty())
        std::cout << "No resources found.\n";
    for (const auto &r : resources)
    {
        std::cout << "ID: " << r.getResourceId() << " | Title: '" << r.getTitle()
                  << "' | Category ID: " << r.getCategoryId()
                  << " | Avail: " << r.getAvailableCopies() << "/" << r.getTotalCopies() << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleAddCategory()
{
    std::string categoryName, description;
    std::cout << "\n--- ADD NEW CATEGORY ---\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter Category Name: ";
    std::getline(std::cin, categoryName);

    std::cout << "Enter Category Description: ";
    std::getline(std::cin, description);

    Category newCategory(0, categoryName, description);

    if (adminService.addCategory(newCategory))
    {
        std::cout << " Category added successfully!\n";
    }
    else
    {
        std::cout << " Database Error: Could not add category.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void AdminMenu::handleEditCategory()
{
    int categoryId;
    std::cout << "\n--- EDIT CATEGORY ---\n";
    std::cout << "Enter Category ID: ";
    if (!(std::cin >> categoryId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::unique_ptr<Category> category = adminService.getCategoryById(categoryId);
    if (!category)
    {
        std::cout << " Error: Category not found.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string tempStr;

    std::cout << "Current Name: " << category->getName() << "\n";
    std::cout << "New Name (or press Enter to keep): ";
    std::getline(std::cin, tempStr);
    if (!tempStr.empty())
        category->setName(tempStr);

    std::cout << "Current Description: " << category->getDescription() << "\n";
    std::cout << "New Description (or press Enter to keep): ";
    std::getline(std::cin, tempStr);
    if (!tempStr.empty())
        category->setDescription(tempStr);

    if (adminService.editCategory(*category))
        std::cout << " Category updated successfully!\n";
    else
        std::cout << " Failed to update category.\n";

    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void AdminMenu::handleDeleteCategory()
{
    int categoryId;
    std::cout << "\n--- DELETE CATEGORY ---\n";
    std::cout << "Enter Category ID: ";
    if (!(std::cin >> categoryId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << " Are you sure you want to delete this category? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        if (adminService.deleteCategory(categoryId))
            std::cout << " Category deleted!\n";
        else
            std::cout << " Failed to delete. Resources might still be linked to it.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllCategories()
{
    std::cout << "\n--- ALL CATEGORIES ---\n";
    std::vector<Category> categories = adminService.viewAllCategories();
    if (categories.empty())
        std::cout << "No categories found.\n";
    for (const auto &c : categories)
    {
        std::cout << "ID: " << c.getCategoryId() << " | Name: " << c.getName()
                  << " | Desc: " << c.getDescription() << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

/* *************************************************************************
                  ---------- MEMBER MANAGEMENT ----------
   ************************************************************************* */

void AdminMenu::displayMemberMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "           MEMBER MANAGEMENT            \n";
        std::cout << "========================================\n";
        std::cout << "1. Add User\n";
        std::cout << "2. Edit User\n";
        std::cout << "3. Delete User\n";
        std::cout << "4. Suspend User\n";
        std::cout << "5. Reactivate User\n";
        std::cout << "6. Process Deletion Requests\n";
        std::cout << "7. View All Users\n";
        std::cout << "0. Back to Main Dashboard\n";
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
            handleAddUser();
            break;
        case 2:
            handleEditUser();
            break;
        case 3:
            handleDeleteUser();
            break;
        case 4:
            handleSuspendUser();
            break;
        case 5:
            handleReactivateUser();
            break;
        case 6:
            handleProcessAccountDeletion();
            break;
        case 7:
            handleViewAllUsers();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleAddUser()
{
    std::string username, password, email, firstName, lastName;
    int membershipTypeId;
    double startingBalance = 0.0;

    std::cout << "\n--- ADD NEW MEMBER ---\n";
    std::cout << "Enter First Name: ";
    std::cin >> firstName;
    std::cout << "Enter Last Name: ";
    std::cin >> lastName;
    std::cout << "Enter Username (min 5 chars): ";
    std::cin >> username;
    std::cout << "Enter Password: ";
    std::cin >> password;
    std::cout << "Enter Email: ";
    std::cin >> email;
    std::cout << "Enter Membership Type ID (1=Basic, 2=Premium): ";

    if (!(std::cin >> membershipTypeId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << " Invalid input for Membership ID.\n";
        return;
    }

    User newUser;
    newUser.setFirstName(firstName);
    newUser.setLastName(lastName);
    newUser.setUsername(username);
    newUser.setPassword(password);
    newUser.setEmail(email);
    newUser.setMembershipTypeId(membershipTypeId);
    newUser.setBalance(startingBalance);
    newUser.setIsActive(true);

    Validator::ValidationResult result = Validator::validate(newUser);

    if (!result.isValid)
    {
        std::cout << "\n Validation Failed:\n";
        for (const std::string &error : result.errors)
        {
            std::cout << "  - " << error << "\n";
        }
    }
    else
    {
        if (adminService.addUser(newUser))
        {
            std::cout << "\n User added successfully!\n";
        }
        else
        {
            std::cout << "\n Database Error: Could not add user (Username or Email might already exist).\n";
        }
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleEditUser()
{
    int userId;
    std::cout << "\n--- EDIT USER ---\n";
    std::cout << "Enter User ID to Edit: ";

    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::unique_ptr<User> user = adminService.getUserById(userId);

    if (!user)
    {
        std::cout << " Error: User ID " << userId << " not found.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }

    std::string tempStr;
    int tempInt;

    std::cout << "\n--- Current User Details ---\n";
    std::cout << "First Name: " << user->getFirstName() << "\n";
    std::cout << "Last Name: " << user->getLastName() << "\n";
    std::cout << "Email: " << user->getEmail() << "\n";
    std::cout << "Membership Type: " << user->getMembershipTypeId() << "\n";
    std::cout << "----------------------------\n";
    std::cout << "Enter new values (or type '.' to keep the current value)\n\n";

    std::cout << "New First Name: ";
    std::cin >> tempStr;
    if (tempStr != ".")
        user->setFirstName(tempStr);

    std::cout << "New Last Name: ";
    std::cin >> tempStr;
    if (tempStr != ".")
        user->setLastName(tempStr);

    std::cout << "New Email: ";
    std::cin >> tempStr;
    if (tempStr != ".")
        user->setEmail(tempStr);

    std::cout << "New Password (or type '.' to keep current): ";
    std::cin >> tempStr;
    if (tempStr != ".")
        user->setPassword(tempStr);

    std::cout << "New Membership Type ID (current: " << user->getMembershipTypeId() << ", enter 0 to keep): ";
    if (!(std::cin >> tempInt))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << " Invalid input. Aborting edit.\n";
        return;
    }
    if (tempInt != 0)
        user->setMembershipTypeId(tempInt);

    Validator::ValidationResult result = Validator::validate(*user);

    if (!result.isValid)
    {
        std::cout << "\n Validation Failed:\n";
        for (const std::string &error : result.errors)
        {
            std::cout << "  - " << error << "\n";
        }
    }
    else
    {
        if (adminService.editUser(*user))
        {
            std::cout << "\n User updated successfully!\n";
        }
        else
        {
            std::cout << "\n Database Error: Could not update user. (Check constraints like unique Email).\n";
        }
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleDeleteUser()
{
    int userId;
    std::cout << "\n--- FORCE DELETE USER ---\n";
    std::cout << "Enter User ID to Delete: ";

    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << " WARNING: This will permanently delete the user. Are you sure? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        if (adminService.deleteUserAccount(userId))
        {
            std::cout << " User permanently deleted.\n";
        }
        else
        {
            std::cout << " Failed to delete user. They may have active borrowed books or unpaid fines.\n";
        }
    }
    else
    {
        std::cout << "Deletion cancelled.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleSuspendUser()
{
    int userId;
    std::cout << "\n--- SUSPEND USER ACCOUNT ---\n";
    std::cout << "Enter User ID to Suspend: ";

    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (adminService.suspendUserAccount(userId))
    {
        std::cout << " User account suspended successfully.\n";
    }
    else
    {
        std::cout << " Failed to suspend account. User ID might not exist.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleReactivateUser()
{
    int userId;
    std::cout << "\n--- REACTIVATE USER ACCOUNT ---\n";
    std::cout << "Enter User ID to Reactivate: ";

    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    if (adminService.reactivateUserAccount(userId))
    {
        std::cout << " User account reactivated successfully.\n";
    }
    else
    {
        std::cout << " Failed to reactivate account. User ID might not exist.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleProcessAccountDeletion()
{
    std::cout << "\n--- PROCESS DELETION REQUESTS ---\n";

    std::vector<User> pendingRequests = adminService.viewDeletionRequests();

    if (pendingRequests.empty())
    {
        std::cout << "No pending deletion requests at this time.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }

    for (const User &u : pendingRequests)
    {
        std::cout << "User ID: " << u.getUserId()
                  << " | Name: " << u.getFirstName() << " " << u.getLastName()
                  << " | Username: " << u.getUsername() << "\n";
    }

    int userId;
    char decision;

    std::cout << "\nEnter User ID to process (or 0 to cancel): ";
    if (!(std::cin >> userId) || userId == 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Approve deletion for User ID " << userId << "? (y/n): ";
    std::cin >> decision;

    bool approve = (decision == 'y' || decision == 'Y');

    if (adminService.processAccountDeletionRequest(userId, approve))
    {
        std::cout << " Deletion request " << (approve ? "APPROVED and user deleted" : "REJECTED and request cleared") << ".\n";
    }
    else
    {
        std::cout << " Error: Could not process request. Please check the User ID.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllUsers()
{
    std::cout << "\n--- ALL REGISTERED MEMBERS ---\n";
    std::vector<User> users = adminService.viewAllUsers();

    if (users.empty())
    {
        std::cout << "No users found in the system.\n";
    }
    else
    {
        for (const User &u : users)
        {
            std::cout << "ID: " << u.getUserId()
                      << " | Name: " << u.getFirstName() << " " << u.getLastName()
                      << " | Username: " << u.getUsername()
                      << " | Active: " << (u.getIsActive() ? "Yes" : "SUSPENDED") << "\n";
        }
    }
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

/* *************************************************************************
                  ---------- CIRCULATION DESK ----------
   ************************************************************************* */

void AdminMenu::displayCirculationMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "           CIRCULATION DESK             \n";
        std::cout << "========================================\n";
        std::cout << "1. View Pending Borrow Requests\n";
        std::cout << "2. Process a Borrow Request\n";
        std::cout << "3. Process a Book Return\n";
        std::cout << "4. View All Transactions\n";
        std::cout << "5. View Transactions By User\n";
        std::cout << "6. View All Reservations\n";
        std::cout << "7. View Reservations By User\n";
        std::cout << "8. Cancel a Reservation\n";
        std::cout << "0. Back to Main Dashboard\n";
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
        {

            std::vector<Transaction> pending = adminService.viewPendingBorrowRequests();
            std::cout << "\n--- Pending Requests ---\n";
            if (pending.empty())
            {
                std::cout << "No pending requests found.\n";
            }
            else
            {
                for (const auto &txn : pending)
                {
                    std::cout << "Txn ID: " << txn.getTransactionId()
                              << " | User ID: " << txn.getUserId()
                              << " | Resource ID: " << txn.getResourceId() << "\n";
                }
            }
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
        break;

        case 2:
            handleProcessBorrowRequest();
            break;

        case 3:
            handleProcessReturn();
            break;
        case 4:
            handleViewAllTransactions();
            break;
        case 5:
            handleViewUserTransactions();
            break;
        case 6:
            handleViewAllReservations();
            break;
        case 7:
            handleViewUserReservations();
            break;
        case 8:
            handleCancelReservation();
            break;

        case 0:
            running = false;
            break;

        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleProcessBorrowRequest()
{
    int txnId;
    char decision;

    std::cout << "\nEnter Transaction ID to Process: ";
    if (!(std::cin >> txnId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Approve this request? (y/n): ";
    std::cin >> decision;

    bool approve = (decision == 'y' || decision == 'Y');

    bool success = adminService.processBorrowRequest(txnId, approve, simulatedToday);

    if (success)
    {
        std::cout << " Request " << (approve ? "APPROVED" : "REJECTED") << " successfully.\n";
    }
    else
    {
        std::cout << " Error: Could not process request. (Resource might be out of stock).\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleProcessReturn()
{
    int txnId;
    std::cout << "\nEnter Transaction ID to Process Return: ";
    if (!(std::cin >> txnId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << " Invalid input. Press Enter to cancel.\n";
        std::cin.get();
        return; // Abort cleanly
    }

    bool success = adminService.processReturn(txnId, simulatedToday);

    if (success)
    {
        std::cout << " Book returned successfully. Inventory updated and fines calculated.\n";
    }
    else
    {
        std::cout << " Error: Could not process return. Check if the Transaction ID is correct and currently ISSUED.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllTransactions()
{
    std::cout << "\n--- ALL TRANSACTIONS ---\n";
    std::vector<Transaction> txns = adminService.viewAllTransactions();
    if (txns.empty())
        std::cout << "No transactions found.\n";
    for (const auto &t : txns)
    {
        std::cout << "Txn ID: " << t.getTransactionId() << " | User: " << t.getUserId()
                  << " | Res: " << t.getResourceId() << " | Status: " << t.getTransactionStatus() << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewUserTransactions()
{
    int userId;
    std::cout << "\nEnter User ID: ";
    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "\n--- TRANSACTIONS FOR USER " << userId << " ---\n";
    std::vector<Transaction> txns = adminService.viewTransactionsByUser(userId);
    if (txns.empty())
        std::cout << "No transactions found for this user.\n";
    for (const auto &t : txns)
    {
        std::cout << "Txn ID: " << t.getTransactionId() << " | Res: " << t.getResourceId()
                  << " | Status: " << t.getTransactionStatus() << " | Due: " << (t.getDueDate().empty() ? "N/A" : t.getDueDate()) << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllReservations()
{
    std::cout << "\n--- ALL RESERVATIONS ---\n";
    std::vector<Reservation> holds = adminService.viewAllReservations();
    if (holds.empty())
        std::cout << "No active reservations found.\n";
    for (const auto &r : holds)
    {
        std::cout << "Hold ID: " << r.getReservationId() << " | User: " << r.getUserId()
                  << " | Res: " << r.getResourceId() << " | Status: " << r.getStatus() << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewUserReservations()
{
    int userId;
    std::cout << "\nEnter User ID: ";
    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "\n--- RESERVATIONS FOR USER " << userId << " ---\n";
    std::vector<Reservation> holds = adminService.viewReservationsByUser(userId);
    if (holds.empty())
        std::cout << "No reservations found for this user.\n";
    for (const auto &r : holds)
    {
        std::cout << "Hold ID: " << r.getReservationId() << " | Res: " << r.getResourceId() << " | Status: " << r.getStatus() << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleCancelReservation()
{
    int resId;
    std::cout << "\nEnter Reservation ID to Cancel: ";
    if (!(std::cin >> resId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    if (adminService.cancelReservation(resId))
    {
        std::cout << " Reservation successfully cancelled.\n";
    }
    else
    {
        std::cout << " Error: Could not cancel reservation. ID might not exist.\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

/* *************************************************************************
                    ---------- FINANCIAL DESK ----------
   ************************************************************************* */

void AdminMenu::displayFinancialMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "           FINANCIAL DESK               \n";
        std::cout << "========================================\n";
        std::cout << "1. Process Fund Requests\n";
        std::cout << "2. View Fines by User\n";
        std::cout << "3. Waive a Fine\n";
        std::cout << "4. Receive Fine Payment\n";
        std::cout << "5. View All System Fines\n";
        std::cout << "6. Impose Manual Fine\n";
        std::cout << "7. Edit Existing Fine\n";
        std::cout << "8. Delete Fine Record\n";
        std::cout << "0. Back to Main Dashboard\n";
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
            handleProcessFundRequest();
            break;
        case 2:
            handleViewUserFines();
            break;
        case 3:
            handleWaiveFine();
            break;
        case 4:
            handleReceiveFinePayment();
            break;
        case 5:
            handleViewAllFines();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleProcessFundRequest()
{
    std::cout << "\n--- PROCESS FUND REQUESTS ---\n";

    std::vector<FundRequest> pendingRequests = adminService.viewPendingFundRequests();

    if (pendingRequests.empty())
    {
        std::cout << "No pending fund requests at this time.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
        return;
    }

    for (const FundRequest &req : pendingRequests)
    {
        if (req.getStatus() == "PENDING")
        {
            std::cout << "Request ID: " << req.getRequestId()
                      << " | User ID: " << req.getUserId()
                      << " | Amount: $" << req.getRequestedAmount() << "\n";
        }
    }

    int reqId;
    char decision;

    std::cout << "\nEnter Request ID to process (or 0 to cancel): ";
    if (!(std::cin >> reqId) || reqId == 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << "Approve this fund request? (y/n): ";
    std::cin >> decision;

    bool approve = (decision == 'y' || decision == 'Y');

    if (adminService.processFundRequest(reqId, approve, simulatedToday))
    {
        std::cout << " Fund request " << (approve ? "APPROVED. Balance updated." : "REJECTED.") << "\n";
    }
    else
    {
        std::cout << " Error: Could not process request. Please check the Request ID.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleViewUserFines()
{
    int userId;
    std::cout << "\n--- VIEW USER FINES ---\n";
    std::cout << "Enter User ID: ";

    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::vector<Fine> userFines = adminService.viewFinesByUser(userId);

    if (userFines.empty())
    {
        std::cout << " No fines found for User ID " << userId << ".\n";
    }
    else
    {
        std::cout << "\n--- Fines for User ID " << userId << " ---\n";
        for (const Fine &fine : userFines)
        {
            std::cout << "Fine ID: " << fine.getFineId()
                      << " | Txn ID: " << fine.getTransactionId()
                      << " | Amount: $" << fine.getFineAmount()
                      << " | Status: " << (fine.getIsPaid() ? "PAID" : "UNPAID") << "\n";
        }
    }

    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleWaiveFine()
{
    int fineId;
    std::cout << "\n--- WAIVE A FINE ---\n";
    std::cout << "Enter Fine ID to Waive: ";

    if (!(std::cin >> fineId))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    std::cout << " Are you sure you want to waive this fine? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        if (adminService.waiveFine(fineId))
        {
            std::cout << " Fine waived successfully! The amount has been cleared.\n";
        }
        else
        {
            std::cout << " Error: Could not waive fine. Check if the Fine ID is correct.\n";
        }
    }
    else
    {
        std::cout << "Action cancelled.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void AdminMenu::handleReceiveFinePayment()
{
    int fineId;
    std::cout << "\n--- RECEIVE FINE PAYMENT ---\n";
    std::cout << "Enter Fine ID to mark as PAID: ";
    if (!(std::cin >> fineId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    if (adminService.markFineAsPaid(fineId))
    {
        std::cout << " Payment received! Fine marked as Paid.\n";
    }
    else
    {
        std::cout << " Error: Could not process payment. Verify the Fine ID.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllFines()
{
    std::cout << "\n--- ALL SYSTEM FINES ---\n";
    std::vector<Fine> fines = adminService.viewAllFines();

    if (fines.empty())
    {
        std::cout << "No fines exist in the system.\n";
    }
    else
    {
        for (const Fine &f : fines)
        {
            std::cout << "Fine ID: " << f.getFineId()
                      << " | User ID: " << f.getUserId()
                      << " | Amount: $" << f.getFineAmount()
                      << " | Status: " << (f.getIsPaid() ? "PAID" : "UNPAID") << "\n";
        }
    }
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleImposeFine()
{
    int userId, transactionId;
    double fineAmount;

    std::cout << "\n--- IMPOSE MANUAL FINE ---\n";
    std::cout << "Enter User ID: ";
    if (!(std::cin >> userId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "Enter Transaction ID (or 0 if this is a general penalty): ";
    if (!(std::cin >> transactionId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "Enter Fine Amount: $";
    if (!(std::cin >> fineAmount) || fineAmount < 0)
    {
        std::cout << " Invalid amount.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    Fine manualFine;
    manualFine.setUserId(userId);
    manualFine.setTransactionId(transactionId);
    manualFine.setFineAmount(fineAmount);
    manualFine.setDaysOverdue(0); // Manual fine, not necessarily tied to overdue days
    manualFine.setFineDate(simulatedToday);
    manualFine.setIsPaid(false);

    if (adminService.imposeFine(manualFine))
    {
        std::cout << " Manual fine imposed successfully!\n";
    }
    else
    {
        std::cout << " Database Error: Could not impose fine. Check if User ID exists.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleUpdateFine()
{
    int fineId;
    std::cout << "\n--- EDIT EXISTING FINE ---\n";
    std::cout << "Enter Fine ID to Edit: ";
    if (!(std::cin >> fineId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::unique_ptr<Fine> targetFine = adminService.getFineById(fineId);

    if (!targetFine)
    {
        std::cout << " Error: Fine ID " << fineId << " not found.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
        return;
    }

    double newAmount;
    std::cout << "\nCurrent Fine Amount: $" << targetFine->getFineAmount() << "\n";
    std::cout << "Status: " << (targetFine->getIsPaid() ? "PAID" : "UNPAID") << "\n";
    std::cout << "Enter New Fine Amount: $";

    if (!(std::cin >> newAmount) || newAmount < 0)
    {
        std::cout << " Invalid amount. Aborting edit.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    targetFine->setFineAmount(newAmount);

    if (adminService.updateFine(*targetFine))
    {
        std::cout << " Fine updated successfully to $" << newAmount << "!\n";
    }
    else
    {
        std::cout << " Database Error: Could not update fine.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleDeleteFine()
{
    int fineId;
    std::cout << "\n--- DELETE FINE RECORD ---\n";
    std::cout << "Enter Fine ID to permanently delete: ";

    if (!(std::cin >> fineId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << "  WARNING: This permanently erases the fine from the database. \n";
    std::cout << "   (If you just want to forgive the debt, use 'Waive Fine' instead).\n";
    std::cout << "   Are you sure? (y/n): ";

    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y')
    {
        if (adminService.deleteFine(fineId))
        {
            std::cout << " Fine record permanently deleted.\n";
        }
        else
        {
            std::cout << " Error: Could not delete fine. Verify the Fine ID.\n";
        }
    }
    else
    {
        std::cout << "Action cancelled.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

/* *************************************************************************
                   ---------- REPORTING ENGINE ----------
   ************************************************************************* */

void AdminMenu::displayReportingMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "           REPORT GENERATION MENU       \n";
        std::cout << "========================================\n";
        std::cout << "1. User Borrowing History Report\n";
        std::cout << "2. Issued/Overdue Resources Report\n";
        std::cout << "0. Back to Main Dashboard\n";
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
            handleGenerateHistoryReport();
            break;
        case 2:
            handleGenerateIssue_OverdueReport();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleGenerateHistoryReport()
{
    std::string filename;

    std::cout << "\n--- USER BORROWING HISTORY REPORT ---\n";
    std::cout << "Enter the name of the file to save (e.g., HistoryReport.pdf): ";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, filename);

    if (filename.empty())
    {
        filename = "UserHistoryReport.pdf";
        std::cout << "No filename provided. Defaulting to: " << filename << "\n";
    }

    if (adminService.generateUserHistoryReport(filename))
    {
        std::cout << " Report generated successfully!\n";
    }
    else
    {
        std::cout << " Error while generating report. Please check file permissions and try again.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void AdminMenu::handleGenerateIssue_OverdueReport()
{
    std::string filename;

    std::cout << "\n--- ISSUED & OVERDUE RESOURCES REPORT ---\n";
    std::cout << "Enter the name of the file to save (e.g., OverdueReport.pdf): ";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, filename);

    if (filename.empty())
    {
        filename = "OverdueReport.pdf";
        std::cout << "No filename provided. Defaulting to: " << filename << "\n";
    }

    if (adminService.generateIssuedAndOverdueReport(filename))
    {
        std::cout << " Report generated successfully!\n";
    }
    else
    {
        std::cout << " Error while generating report. Please check file permissions and try again.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.get();
}

/* *************************************************************************
                 ---------- SYSTEM & ADMIN SETTINGS ----------
   ************************************************************************* */

void AdminMenu::displaySystemMenu()
{
    int choice = -1;
    bool running = true;

    while (running)
    {
        ConsoleUtils::clearScreen();
        std::cout << "========================================\n";
        std::cout << "       SYSTEM & ADMIN SETTINGS          \n";
        std::cout << "========================================\n";
        std::cout << "--- Membership Tier Management ---\n";
        std::cout << "1. View All Membership Types\n";
        std::cout << "2. Add Membership Type\n";
        std::cout << "3. Edit Membership Type\n";
        std::cout << "4. Delete Membership Type\n";
        std::cout << "\n--- Administrator Management ---\n";
        std::cout << "5. View All Administrators\n";
        std::cout << "6. Add New Administrator\n";
        std::cout << "7. Delete Administrator Account\n";
        std::cout << "0. Back to Main Dashboard\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            handleViewAllMembershipTypes();
            break;
        case 2:
            handleAddMembershipType();
            break;
        case 3:
            handleEditMembershipType();
            break;
        case 4:
            handleDeleteMembershipType();
            break;
        case 5:
            handleViewAllAdministrators();
            break;
        case 6:
            handleAddAdministrator();
            break;
        case 7:
            handleDeleteAdministrator();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press Enter to try again.\n";
            std::cin.ignore(10000, '\n');
            std::cin.get();
            break;
        }
    }
}

void AdminMenu::handleViewAllMembershipTypes()
{
    std::cout << "\n--- MEMBERSHIP TIERS ---\n";
    std::vector<MembershipType> types = adminService.viewAllMembershipTypes();
    if (types.empty())
        std::cout << "No membership tiers found.\n";
    for (const auto &t : types)
    {
        std::cout << "ID: " << t.getMembershipTypeId() << " | Name: " << t.getMembershipName()
                  << " | Price: $" << t.getPrice() << " | Max Borrow: " << t.getMaxBorrowingLimit() << " items\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleAddMembershipType()
{
    std::string name, desc;
    int duration, borrowLimit, borrowDuration;
    double price, fine;

    std::cout << "\n--- ADD MEMBERSHIP TIER ---\n";
    std::cin.ignore(10000, '\n');

    std::cout << "Tier Name (e.g., Premium): ";
    std::getline(std::cin, name);
    std::cout << "Duration (Days, e.g., 365): ";
    std::cin >> duration;
    std::cout << "Price ($): ";
    std::cin >> price;
    std::cout << "Max Items Allowed to Borrow: ";
    std::cin >> borrowLimit;
    std::cout << "Standard Borrow Duration (Days): ";
    std::cin >> borrowDuration;
    std::cout << "Daily Fine for Late Returns ($): ";
    std::cin >> fine;

    std::cin.ignore(10000, '\n');
    std::cout << "Description: ";
    std::getline(std::cin, desc);

    MembershipType newTier(0, name, duration, price, borrowLimit, borrowDuration, fine, desc);

    if (adminService.addMembershipType(newTier))
        std::cout << " Tier added successfully!\n";
    else
        std::cout << " Database Error.\n";

    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void AdminMenu::handleEditMembershipType()
{
    int typeId;
    std::cout << "\nEnter Membership Type ID to edit: ";
    if (!(std::cin >> typeId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::unique_ptr<MembershipType> tier = adminService.getMembershipTypeById(typeId);

    if (!tier)
    {
        std::cout << " Error: Membership Type ID " << typeId << " not found.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
        return;
    }

    std::string tempStr;
    int tempInt;
    double tempDouble;

    std::cout << "\n--- Current Tier Details ---\n";
    std::cout << "Name: " << tier->getMembershipName() << "\n";
    std::cout << "Price: $" << tier->getPrice() << "\n";
    std::cout << "Max Borrowing Limit: " << tier->getMaxBorrowingLimit() << "\n";
    std::cout << "----------------------------\n";
    std::cout << "Enter new values (Type '.' for text or '0' for numbers to keep current value)\n\n";

    std::cin.ignore(10000, '\n');

    std::cout << "New Name: ";
    std::getline(std::cin, tempStr);
    if (tempStr != ".")
        tier->setMembershipName(tempStr);

    std::cout << "New Description: ";
    std::getline(std::cin, tempStr);
    if (tempStr != ".")
        tier->setDescription(tempStr);

    std::cout << "New Price ($): ";
    if (std::cin >> tempDouble && tempDouble != 0)
        tier->setPrice(tempDouble);

    std::cout << "New Max Borrow Limit: ";
    if (std::cin >> tempInt && tempInt != 0)
        tier->setMaxBorrowingLimit(tempInt);

    std::cout << "New Borrow Duration (Days): ";
    if (std::cin >> tempInt && tempInt != 0)
        tier->setBorrowingDurationDays(tempInt);

    std::cout << "New Daily Fine ($): ";
    if (std::cin >> tempDouble && tempDouble != 0)
        tier->setFinePerDay(tempDouble);

    if (adminService.editMembershipType(*tier))
    {
        std::cout << "\n Membership Tier updated successfully!\n";
    }
    else
    {
        std::cout << "\n Database Error: Could not update tier.\n";
    }

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleDeleteMembershipType()
{
    int typeId;
    std::cout << "\nEnter Membership Type ID to delete: ";
    if (!(std::cin >> typeId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::cout << " Proceed with deletion? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if ((confirm == 'y' || confirm == 'Y') && adminService.deleteMembershipType(typeId))
    {
        std::cout << " Tier deleted.\n";
    }
    else
    {
        std::cout << " Deletion failed or cancelled.\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleViewAllAdministrators()
{
    std::cout << "\n--- SYSTEM ADMINISTRATORS ---\n";
    std::vector<Administrator> admins = adminService.viewAllAdministrators();
    if (admins.empty())
        std::cout << "No admins found.\n";
    for (const auto &a : admins)
    {
        std::cout << "Admin ID: " << a.getAdminId() << " | Name: " << a.getFirstName() << " " << a.getLastName()
                  << " | Username: " << a.getUsername() << " | Active: " << (a.getIsActive() ? "Yes" : "No") << "\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleAddAdministrator()
{
    std::string username, password, email, first, last;
    std::cout << "\n--- CREATE SYSTEM ADMINISTRATOR ---\n";
    std::cout << "First Name: ";
    std::cin >> first;
    std::cout << "Last Name: ";
    std::cin >> last;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;
    std::cout << "Email: ";
    std::cin >> email;

    Administrator newAdmin;
    newAdmin.setFirstName(first);
    newAdmin.setLastName(last);
    newAdmin.setUsername(username);
    newAdmin.setPassword(password);
    newAdmin.setEmail(email);
    newAdmin.setCreatedDate(simulatedToday);
    newAdmin.setIsActive(true);

    if (adminService.addAdministrator(newAdmin))
        std::cout << " Admin account created!\n";
    else
        std::cout << " Failed. Username or Email might be taken.\n";

    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void AdminMenu::handleDeleteAdministrator()
{
    int adminId;
    std::cout << "\n--- DELETE ADMINISTRATOR ---\n";
    std::cout << "Enter Admin ID to permanently delete: ";
    if (!(std::cin >> adminId))
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    if (adminId == currentAdminId)
    {
        std::cout << " You cannot delete your own account while logged in!\n";
    }
    else
    {
        std::cout << " Are you sure? (y/n): ";
        char confirm;
        std::cin >> confirm;
        if ((confirm == 'y' || confirm == 'Y') && adminService.deleteAdministrator(adminId))
        {
            std::cout << " Admin deleted.\n";
        }
        else
        {
            std::cout << " Deletion failed or cancelled.\n";
        }
    }
    std::cout << "Press Enter to continue...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}