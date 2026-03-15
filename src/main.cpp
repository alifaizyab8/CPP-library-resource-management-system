#include <iostream>
#include <string>

// Database
#include "infrastructure/database/DatabaseInitializer.h"

// Repositories
#include "infrastructure/repositories/UserRepository.h"
#include "infrastructure/repositories/AdministratorRepository.h"
#include "infrastructure/repositories/ResourceRepository.h"
#include "infrastructure/repositories/CategoryRepository.h"
#include "infrastructure/repositories/TransactionRepository.h"
#include "infrastructure/repositories/FineRepository.h"
#include "infrastructure/repositories/BorrowingHistoryRepository.h"
#include "infrastructure/repositories/FundRequestRepository.h"
#include "infrastructure/repositories/MembershipTypeRepository.h"
#include "infrastructure/repositories/ReservationRepository.h"

// Services
#include "services/AuthenticationService.h"
#include "services/UserService.h"
#include "services/AdminService.h"

// Presentation
#include "presentation/Session.h"
#include "presentation/AuthMenu.h"
#include "presentation/UserMenu.h"
// #include "presentation/AdminMenu.h"  >>> will be uncommented once work on this is completed

// Utilities
#include "Utility/date.h"

int main()
{
    // ==========================================
    // 1. BOOT SEQUENCE (Initialization)
    // ==========================================

    // Initialize SQLite Database
    DatabaseInitializer startDBService("../src/db/library.db");
    if (!startDBService.open())
    {
        std::cerr << "CRITICAL ERROR: Failed to open database.\n";
        return 1;
    }

    // Create tables if they do not exist
    if (!startDBService.createTables())
    {
        std::cerr << "CRITICAL ERROR: Failed to create database tables.\n";
        return 1;
    }

    sqlite3 *db = startDBService.getConnection();

    // Create repository instances
    UserRepository userRepo(db);
    AdministratorRepository adminRepo(db);
    ResourceRepository resourceRepo(db);
    CategoryRepository categoryRepo(db);
    TransactionRepository transactionRepo(db);
    FineRepository fineRepo(db);
    BorrowingHistoryRepository historyRepo(db);
    FundRequestRepository fundReqRepo(db);
    MembershipTypeRepository membershipRepo(db);
    ReservationRepository reservationRepo(db); // Not implemented into program yet

    // Create service instances
    AuthenticationService authService(userRepo, adminRepo);

    UserService userService(userRepo, resourceRepo, transactionRepo,
                            fineRepo, historyRepo, fundReqRepo, membershipRepo);

    AdminService adminService(userRepo, fineRepo, resourceRepo, categoryRepo,
                              fundReqRepo, transactionRepo, reservationRepo,
                              membershipRepo, historyRepo);

    // ==========================================
    // 2. MOCK CLOCK & PRE-COMPUTATION
    // ==========================================

    std::cout << "========================================\n";
    std::cout << "   SYSTEM INITIALIZATION (MOCK CLOCK)   \n";
    std::cout << "========================================\n";

    // utility function
    std::string systemDate = getCurrentDate();
    std::cout << "\n[System] Synchronizing database states...\n";
    std::cout << "[System] Calculating overdues and updating daily fines for " << systemDate << "...\n";

    // Pre-computation: Synchronize DB states prior to user interaction
    adminService.updateDailyFines(systemDate);

    std::cout << "[System] Boot sequence complete. Launching interface...\n\n";

    // ==========================================
    // 3. THE OUTER LOOP (Application Lifecycle)
    // ==========================================

    AuthMenu authMenu(authService, systemDate);
    bool running = true;

    while (running)
    {
        // 4. THE AUTHENTICATION GATEWAY (Routing)
        ActiveSession session = authMenu.displayMenu();

        if (session.isExit)
        {
            // Exit signal received
            running = false;
            std::cout << "\nTerminating application safely. Goodbye!\n";
        }
        else if (session.userId != -1)
        {
            // 5. THE INNER LOOP: Route to User Dashboard
            UserMenu userMenu(userService, session.userId, systemDate);
            userMenu.handleUserUI(); // Traps execution until User logs out
        }
        else if (session.adminId != -1)
        {
            // 5. THE INNER LOOP: Route to Admin Dashboard
            // AdminMenu adminMenu(adminService, session.adminId, systemDate);
            // adminMenu.displayMenu(); // Traps execution until Admin logs out

            std::cout << "\n[Admin Dashboard Placeholder]\n";
            std::cout << "Admin menu is currently under construction.\n";
            std::cout << "Press Enter to log out...";
            std::cin.ignore();
            std::cin.get();
        }
    }

    return 0; // startDBService's destructor safely closes the SQLite connection
}