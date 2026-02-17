#include <iostream>
#include "infrastructure/database/SqliteConnection.h"
#include "infrastructure/repositories/UserRepository.h"
#include "domain/entities/User.h"
#include <vector>
#include <string>

using namespace std;

int main()
{
    cout<<"Please work";
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
