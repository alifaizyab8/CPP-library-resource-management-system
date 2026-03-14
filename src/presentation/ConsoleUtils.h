#pragma once
#include <cstdlib> // Required for std::system

namespace ConsoleUtils
{
    // Automatically detects OS and clears the screen
    inline void clearScreen()
    {
#ifdef _WIN32
        std::system("cls"); // Windows
#else
        std::system("clear"); // Linux / Mac OS
#endif
    }
}