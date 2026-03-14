#pragma once


struct ActiveSession 
{

    int userId = -1;    // -1 means no user is logged in 
    int adminId = -1;   
    bool isExit = false; // True if press 0 to quit the app

};