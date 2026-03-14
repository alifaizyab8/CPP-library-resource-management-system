# Application Lifecycle & Boot Sequence

## Overview

This document describes the full execution flow of the Library Management System from initialization through runtime session management.

---

## 1. Boot Sequence (Initialization)

- **Action:** System executes one-time setup before any UI is rendered.
- **Operations:** Initializes SQLite database connections, instantiates all Repositories and Services, and prompts for the simulated system date.
- **Pre-computation:** Executes `adminService.updateDailyFines(date)` to synchronize database states (overdues/fines) prior to user interaction.

---

## 2. The Outer Loop (Application Lifecycle)

- **Action:** A core `while (systemIsRunning)` loop encapsulates the entire runtime.
- **Purpose:** Ensures the application remains active after a user logs out, continuously resetting to the authentication state for the next session without terminating the executable.

---

## 3. The Authentication Gateway (Routing)

- **Action:** Renders `AuthMenu` and blocks execution until a user authenticates or exits.
- **Output:** Returns an `ActiveSession` struct containing state flags (`isExit`, `adminId`, `userId`).

**Routing Logic:**

| Condition | Outcome |
|---|---|
| `isExit == true` | Sets `systemIsRunning = false`, breaking the outer loop and terminating the program safely. |
| `adminId != -1` | Routes thread control to `AdminMenu`. |
| `userId != -1` | Routes thread control to `UserMenu`. |

---

## 4. The Inner Loop (Role-Specific Dashboards)

- **Action:** `main.cpp` hands control to the respective dashboard module (e.g., `AdminMenu::displayDashboard`).
- **Execution:** The dashboard traps the user in a localized `while` loop, handling all sub-menu navigation (Circulation, Financials, Catalog) independently of `main.cpp`.

---

## 5. The Logout Event (Session Termination)

- **Action:** User selects "Logout" within their specific dashboard.
- **Execution:** The dashboard's inner `while` loop breaks, and the function returns control back up to `main.cpp`.
- **Reset:** The outer loop continues to its next iteration, clearing the previous `ActiveSession` data and rendering the `AuthMenu` for the next user.


---


 **Disclaimer** The Workflow can be altered if required during the implementation of UI