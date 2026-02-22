#include <iostream>
#include <sqlite3.h>

using namespace std;

// Forward declarations
bool testAdministratorRepository(sqlite3 *db);
bool testFineRepository(sqlite3 *db);
bool testFundRequestRepository(sqlite3 *db);
bool testMembershipTypeRepository(sqlite3 *db);
bool testReservationRepository(sqlite3 *db);
bool testTransactionRepository(sqlite3 *db);
bool testUserRepository(sqlite3 *db);

int main()
{
    sqlite3 *db;

    // Open the blank RAM database
    if (sqlite3_open(":memory:", &db) != SQLITE_OK)
    {
        std::cerr << "Cannot open memory database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    cout << "========== RUNNING TEST SUITE ==========" << endl;

    // Run tests one by one
    bool adminPassed = testAdministratorRepository(db);
    bool finePassed = testFineRepository(db);
    bool fundPassed = testFundRequestRepository(db);
    bool membershipPassed = testMembershipTypeRepository(db);
    bool reservationPassed = testReservationRepository(db);
    bool transactionPassed = testTransactionRepository(db);
    bool userPassed = testUserRepository(db);


    cout << "========================================" << endl;

    // Final Report
    if (adminPassed && finePassed && fundPassed && membershipPassed && reservationPassed && transactionPassed && userPassed)
    {
        cout << " SUCCESS: All tested repositories are perfectly bug-free!" << endl;
    }
    else
    {
        cout << " WARNING: One or more repositories failed." << endl;
    }

    sqlite3_close(db);
    return 0;
}
// { //statement for test
//     SqliteConnection db("Library.db");
//     db.connect();
//     UserRepository repo(db.getConnection());

//      repo.addUser("Amna", "amna@test.com");
//     repo.addUser("Maya", "maya@test.com");

//      vector<User> users;
//     cout << "All Users: " << endl;
//      for (User u : users)
//     {
//          cout << u.getUserId() << " | " << u.getUsername() << " | " << u.getAddress() << endl;
//      }

//      vector<User> searchResults = repo.searchUsers("Amna");
//      cout << "Search results for 'Amna': " << endl;
//      for (User u : searchResults)
//      {
//          cout << u.getUserId() << " | " << u.getUsername() << " | " << u.getAddress() << endl;
//      }
//      repo.deleteUser(1);
//      repo.updateUser(2, "Maya Ali", "maya.ali@example.com");
//      db.disconnect();
//      return 0;
// }
