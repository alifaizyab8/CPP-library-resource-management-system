#pragma once
#include <string>

// Forward declaration of the service (IWYU - Include What You Use)
// This prevents circular dependencies and speeds up compilation.
class AdminService;

class AdminMenu
{
private:
    // Dependency Injection
    AdminService &adminService;
    std::string simulatedToday;
    int currentAdminId; // Tracks which admin is currently logged in

    // ==========================================
    // MODULES / SUB-MENUS
    // ==========================================
    void displayCatalogMenu(); // 4th Done
    void displayMemberMenu(); // 2nd Done
    void displayCirculationMenu(); // 3rd Done
    void displayFinancialMenu(); // 5th Done
    void displayReportingMenu(); // 1st Done


    // ==========================================
    // WORKFLOW HANDLERS (Circulation Desk)
    // ==========================================
    void handleProcessBorrowRequest();
    void handleProcessReturn();

    // ==========================================
    // WORKFLOW HANDLERS
    // ==========================================
    // Catalog
    void handleAddResource();
    void handleEditResource();
    void handleDeleteResource();
    void handleViewAllResources();
    void handleViewAllCategories();

    //Category
    void handleAddCategory();
    void handleEditCategory();
    void handleDeleteCategory();

    // Members
    void handleAddUser();
    void handleEditUser();   
    void handleDeleteUser(); 
    void handleSuspendUser();
    void handleReactivateUser(); 
    void handleProcessAccountDeletion();
    void handleViewAllUsers();

    // Financials
    void handleProcessFundRequest();
    void handleWaiveFine();
    void handleViewUserFines();
    void handleReceiveFinePayment();
    void handleViewAllFines();
    void handleImposeFine();
    void handleUpdateFine();
    void handleDeleteFine();

    void handleViewAllTransactions();
    void handleViewUserTransactions();
    void handleViewAllReservations();
    void handleViewUserReservations();
    void handleCancelReservation();

    void displaySystemMenu();

    // Reports
    void handleGenerateHistoryReport();
    void handleGenerateIssue_OverdueReport();


    void handleViewAllAdministrators();
    void handleAddAdministrator();
    void handleDeleteAdministrator();

    void handleViewAllMembershipTypes();
    void handleAddMembershipType();
    void handleEditMembershipType();
    void handleDeleteMembershipType();

public :
    // Constructor
    AdminMenu(AdminService &service, const std::string &today);

    // Main Entry Point
    void displayDashboard(int adminId);
};

