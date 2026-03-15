# AdminService — Full Documentation

## Overview

The `AdminService` is the central business logic layer for all administrator operations in the Library Management System. It acts as the bridge between the UI layer and the underlying database repositories, ensuring that no UI component ever writes SQL directly or manages raw database IDs.

> **Important:** The service layer never communicates with the user directly. No `cout` or `cin` statements belong here — all input/output is handled at the UI layer.

---

## Function Reference

### Catalog & Category Management

```cpp
bool addResource(Resource &resource);
bool editResource(Resource &updatedResource);
bool deleteResource(int resourceId);
std::unique_ptr<Resource> getResourceById(int resourceId);

bool addCategory(Category &category);
bool editCategory(Category &updatedCategory);
bool deleteCategory(int categoryId);
std::unique_ptr<Category> getCategoryById(int categoryId);
```

### User Management

```cpp
bool addUser(User &user);
bool editUser(User &updatedData);
bool deleteUserAccount(int userId);
bool suspendUserAccount(int userId);
bool reactivateUserAccount(int userId);
std::vector<User> viewDeletionRequests();
```

### Request Approvals

```cpp
std::vector<Transaction> viewPendingBorrowRequests();
std::vector<FundRequest> viewPendingFundRequests();
```

### Fine Management

```cpp
std::vector<Fine> viewAllFines();
std::vector<Fine> viewFinesByUser(int userId);
bool imposeFine(Fine &fine);
bool updateFine(Fine &fine);
bool markFineAsPaid(int fineId);
bool waiveFine(int fineId);
bool deleteFine(int fineId);
void updateDailyFines(const std::string &simulatedToday);
```

### Reporting

```cpp
bool generateUserHistoryReport(const std::string &filename);
bool generateIssuedAndOverdueReport(const std::string &filename);
```

### Process Workflows

```cpp
bool processBorrowRequest(int transactionId, bool approve, std::string &dateToday);
bool processReturn(int transactionId, std::string &dateToday);
bool processFundRequest(int fundRequestId, bool approve, std::string &dateToday);
bool processAccountDeletionRequest(int userId, bool approve);
```

---

## Straightforward Functions

The following functions contain no business logic beyond a direct delegation to the repository layer. They exist purely to maintain a clean, readable service API.

`addResource`, `editResource`, `deleteResource`, `getResourceById`, `getCategoryById`, `viewPendingFundRequests`, `viewAllFines`, `viewFinesByUser`, `imposeFine`, `updateFine`

Each of these simply calls its corresponding repository function and returns the result directly.

---

## User Management

### Suspend & Reactivate

Both `suspendUserAccount` and `reactivateUserAccount` follow the same pattern: they fetch the user by ID from the repository, toggle the `isActive` flag to `false` or `true` respectively, and save the updated object back to the database.

### viewDeletionRequests

To retrieve all users with a pending deletion request, the service calls a dedicated repository function `getPendingDeletionRequests()` on the `UserRepository`. This keeps the SQL query inside the repository layer, preserving layered architecture. The service function is simply a wrapper around this call.

---

## Request Approvals

### viewPendingBorrowRequests

This function calls `getByStatus("PENDING")` on the `TransactionRepository`, which returns all transactions currently awaiting admin approval. The service wraps this call and returns the result to the UI layer for display.

---

## Fine Management

### markFineAsPaid

Leaves the fine amount intact for record-keeping purposes but flips the `isPaid` boolean to `true`. The financial history is preserved; only the payment status changes.

### waiveFine

An administrative override. The function fetches the fine record, resets the debt amount to `$0.00`, and marks it as paid. This approach preserves the audit trail — the record confirms a fine was issued — while making clear it was administratively cleared.

### deleteFine

A destructive operation that permanently removes a fine record from the database. This should only be used in cases such as accidental duplicate entries, where the fine itself should never have existed.

### updateDailyFines

This is one of the most critical functions in the system. It is called at the start of each admin session to calculate and synchronize all outstanding fines against the simulated current date. Here is the full workflow:

**Step 1 — Fetch Active Transactions**

Calls `getActiveIssues()` on the `TransactionRepository` to retrieve all transactions that are currently in an issued (not yet returned) state.

**Step 2 — Check Overdue Status**

For each transaction, the function compares the current date against the transaction's due date. Because dates are stored in `YYYY-MM-DD` format (ISO 8601), C++ can compare them lexicographically using standard `<` and `>` operators — no date parsing library is required.

**Step 3 — Calculate Days Overdue**

If the transaction is overdue, the function calls `calculateDaysOverdue()` to determine how many days past the due date the item currently is.

**Step 4 — Apply the Fine Rate**

The system charges a flat rate of **$5.00 per overdue day**.

**Step 5 — Update or Create the Fine Record**

If a fine record already exists for this transaction, it is updated with the new calculated amount. If no fine record exists yet, a new one is created.

**Step 6 — Persist Changes**

All modified or newly created fine records are saved to the database.

---

## Reporting

### User Borrowing History Report

**Function:** `generateUserHistoryReport(const std::string &filename)`

1. Fetches all registered `User` objects from the repository.
2. Iterates through each user and queries the `BorrowingHistoryRepository` using their `userId`.
3. Users with no borrowing history are skipped using `continue` to avoid generating empty data blocks in the report.
4. For every history record, the `ResourceRepository` is queried to translate the raw `resourceId` into a human-readable book title.
5. Empty database fields (such as a due date not yet assigned) are handled with inline ternary operators, so the report prints `"Pending"` instead of a blank space.

### Issued and Overdue Resources Report

**Function:** `generateIssuedAndOverdueReport(const std::string &filename)`

1. Fetches only active (currently issued) transactions. If none exist, a clean "No active resources" report is generated immediately and the function exits — keeping memory usage at O(1) in empty states.
2. Two separate `std::stringstream` buffers are initialized: `issuedSection` and `overdueSection`, so the two categories can be built and assembled independently.
3. For each transaction, `std::unique_ptr` is used to safely fetch the associated `Resource` and `User`. If a linked record has been deleted from the database, the report defaults to `"Unknown Resource"` rather than crashing on a null pointer dereference.
4. Integer values are converted using `std::to_string()`, and each entry is routed into the correct buffer based on the `isOverdue` flag.
5. The final report is assembled by concatenating the overdue buffer followed by the issued buffer, with count summaries prepended to each section header before the output is sent to the PDF generator.

---

## Process Workflows

### processFundRequest

Handles the approval or rejection of a user's fund request.

1. **Validation:** Fetches the `FundRequest` by ID. If the record does not exist or its status is not `PENDING`, the function aborts immediately.
2. **Date Stamping:** Sets the `approvalDate` field to the simulated current date.
3. **Begin Transaction:** Calls `beginTransaction()` to lock the database state before making any changes.
4. **If Approved:**
   - Fetches the `User` who submitted the request. If the user no longer exists, the database transaction is rolled back.
   - Calculates the new balance (`currentBalance + requestedAmount`) and updates the user object.
   - Sets the fund request status to `"APPROVED"`.
   - **Auto-Upgrade:** If the user's new balance reaches or exceeds `$50.00` and they are currently a Basic member (Type 1), the system automatically upgrades their membership to Premium (Type 2).
   - Attempts to save both the updated `User` and the updated `FundRequest` to the database.
5. **If Rejected:**
   - Sets the request status to `"REJECTED"`.
   - Saves only the updated `FundRequest` to the database.

---

### processReturn

Handles the physical return of a borrowed item and updates all related records atomically.

1. **Validation:** Fetches the transaction by `transactionId`. If it does not exist or its status is not `"ISSUED"`, the function aborts immediately.
2. **Begin Transaction:** Calls `beginTransaction()` to ensure the following updates occur as an all-or-nothing atomic operation.
3. **Update Transaction:** Sets the transaction status to `"RETURNED"`, flips the return flag, and stamps today's date.
4. **Restore Inventory:** Fetches the borrowed resource and increments `availableCopies` by 1. If this save fails, the function calls `rollbackTransaction()` and aborts.
5. **Update Borrowing History:** Fetches the user's `BorrowingHistory` and locates the specific record matching this book that has no return date (`returnDate.empty()`). The return date is stamped and any pending fine amount is recorded on the history entry. If this save fails, `rollbackTransaction()` is called and the function aborts.
6. **Final Save & Commit:** Saves the updated `Transaction` object. On success, `commitTransaction()` is called to permanently write all three table updates (Transaction, Resource, BorrowingHistory) to disk. On failure, `rollbackTransaction()` is called to ensure the system never shows a book as returned while the transaction record still reads "ISSUED".

---

### processBorrowRequest

Handles the approval or rejection of a user's borrow request.

1. **Validation:** Fetches the pending transaction by `transactionId`. If it does not exist, the function aborts immediately.
2. **Begin Transaction:** Calls `beginTransaction()` to ensure all subsequent database changes happen atomically.
3. **If Approved:**
   - Sets the transaction status to `"ISSUED"`, stamps today's date, and calculates the due date (14 days from today).
   - Fetches the requested `Resource` and verifies it is both active and in stock (`availableCopies > 0`). If the resource is unavailable, `rollbackTransaction()` is called and the function aborts.
   - Decrements `availableCopies` by 1 and saves the resource. If this save fails, the function rolls back and aborts.
   - Creates and saves a new `BorrowingHistory` record for the user. If this save fails, the function rolls back and aborts.
4. **If Rejected:**
   - Skips all inventory changes and simply sets the transaction status to `"REJECTED"`.
5. **Final Save & Commit:** Saves the updated `Transaction` record. On success, `commitTransaction()` permanently writes all changes. On failure, `rollbackTransaction()` undoes any inventory deductions or history entries that were made during this operation.

---

### processAccountDeletionRequest

Handles the approval or rejection of a user-submitted account deletion request.

1. **Validation:** Fetches the `User` record by `userId`. If the user does not exist, the function aborts immediately.
2. **If Approved:** Calls `deleteUserAccount(userId)`, which permanently removes the user record from the database. Returns `true` on success.
3. **If Rejected:** Sets the `deletionRequested` flag on the user object back to `false`, clearing the pending request. Saves the updated user record and returns the success status of that save operation.



# Design Principles & Engineering Decisions

This section documents the core engineering decisions behind the `AdminService`. Each principle was chosen deliberately to keep the system stable, testable, and maintainable as the codebase grows.

---

## 1. Architecture & Design Patterns

**Dependency Injection (DI)**
All database repositories are injected into the service through constructor references rather than being created internally. This strictly enforces the Single Responsibility Principle, the service owns business logic, and nothing else.

**Facade Pattern**
The service exposes a clean, human-readable API to the UI layer. No UI component ever sees raw SQL, database IDs, or repository internals. The service translates intent into data operations silently.

**State Machine Enforcement**
Resource and transaction lifecycles follow strictly controlled state transitions. A transaction moves from `PENDING` to `ISSUED` to `RETURNED` in that order, and no step can be skipped or reversed arbitrarily. This keeps system state predictable at all times.

---

## 2. Memory & Performance

**Smart Pointer Safety**
All single-record database queries return `std::unique_ptr`. This guarantees automatic memory cleanup when the pointer goes out of scope and allows immediate null-checking, if the record does not exist, the pointer is `nullptr` and the function exits safely before any further logic runs.


**Optimized Memory Buffering**
Report generation uses `std::stringstream` for building large text outputs rather than repeated string concatenation. Standard concatenation creates a new allocation on every append, `stringstream` eliminates that overhead entirely.

**Pass-by-Reference for State Sync**
Domain objects are passed to repository save functions via non-const reference. When SQLite generates a new auto-increment ID after an insert, the repository writes that ID directly back into the original C++ object. The caller immediately has the new ID without any extra fetch.

---

## 3. Data Integrity & Business Rules

**Atomic Workflows**
Any operation that touches multiple tables — such as returning a book, restoring its inventory count, and logging the history — is wrapped in a database transaction. Either all three succeed, or none of them do. This prevents corrupted states like a book showing as returned while its inventory count was never restored.

**Idempotent Daily Fine Calculation**
The `updateDailyFines` function is designed to be run repeatedly without side effects. When it detects an existing fine for a transaction, it updates the amount rather than creating a duplicate record. Running it twice on the same date produces exactly the same database state as running it once.

**Immutable Audit Trails**
Financial and historical records are never silently deleted. Waiving a fine zeroes the balance and marks it paid, but the fine record itself remains in the database. Similarly, approving a borrow request immediately creates a permanent `BorrowingHistory` entry — the archive exists from the moment the book is issued, not only after it is returned.

**Upsert Abstraction for API Clarity**
The repository layer uses a single `save()` function that internally decides whether to run an `INSERT` or `UPDATE` based on whether the object's ID is zero. The service layer, however, exposes distinct `add()` and `edit()` functions. This keeps the UI integration unambiguous — a developer calling the service always knows exactly what operation they are triggering.

---

## 4. Code Quality & Testability

**Time-Travel Testing**
No business logic reads directly from the system clock. Instead, a simulated date string is injected at runtime and passed through to every function that depends on it. This makes due date calculation, overdue detection, and fine accumulation fully deterministic, any future or past date scenario can be tested without manipulating the hardware clock.

**Defensive Programming**
Every function that fetches a record checks the result before proceeding. Early exits on null pointers, missing records, or wrong status codes prevent cascading failures. Report generation handles missing relational data, such as a deleted resource still referenced in a transaction — by defaulting to safe fallback strings rather than crashing.

**DRY Principle**
Shared logic is written once. Report formatting routines, overdue detection, and fine calculation are centralized rather than duplicated across functions. This keeps each individual function short, focused, and easy to modify without risking unintended side effects elsewhere.