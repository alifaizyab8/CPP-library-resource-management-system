# 🏛️ Library & Resource Management System

<div align="center">

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&size=28&duration=3000&pause=1000&color=4FC3F7&center=true&vCenter=true&width=600&lines=Library+Resource+Management+System;Built+with+C%2B%2B17+%2B+SQLite3;Enterprise-Grade+Architecture;N-Tier+%7C+DDD+%7C+Repository+Pattern" alt="Typing SVG" />

<br/>

[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![SQLite](https://img.shields.io/badge/SQLite3-Embedded-003B57?style=for-the-badge&logo=sqlite&logoColor=white)](https://sqlite.org/)
[![Architecture](https://img.shields.io/badge/Architecture-N--Tier-8B5CF6?style=for-the-badge&logo=buffer&logoColor=white)]()
[![License](https://img.shields.io/badge/License-MIT-22C55E?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-0EA5E9?style=for-the-badge&logo=linux&logoColor=white)]()

<br/>

> **Enterprise-grade console application** engineered with Domain-Driven Design, ACID-compliant SQLite3 persistence, automated PDF reporting, and a bulletproof financial engine — built collaboratively as a semester capstone project.

<br/>

[📋 Features](#-key-features) • [🏗️ Architecture](#-system-architecture) • [🚀 Quick Start](#-getting-started) • [👥 Team](#-core-development-team)

</div>

---

## 📖 About The Project

The **Library & Resource Management System (LRMS)** is a high-performance, console-based enterprise application built to digitize and automate modern library workflows. Engineered with strict **Domain-Driven Design (DDD)** and **Object-Oriented Programming (OOP)** principles, the system delivers a decoupled, N-Tier architecture that cleanly separates business logic, data access, and presentation concerns.

Moving beyond basic file handling, this system guarantees **ACID-compliant** data persistence via an embedded SQLite3 relational database. It features a robust financial engine for fine calculation, automated PDF reporting, and bulletproof temporal logic to handle complex real-world edge cases.

```
╔══════════════════════════════════════════════════════════════╗
║         LIBRARY & RESOURCE MANAGEMENT SYSTEM v1.0            ║
║            Powered by C++17 · SQLite3 · CMake                ║
╚══════════════════════════════════════════════════════════════╝
```

---

## ✨ Key Features

<table>
<tr>
<td width="50%">

### 🔐 Security & Access
- **Role-Based Access Control (RBAC)** — Separate portals for Super Admins and Members
- **Parameterized SQL Queries** — Complete SQL injection prevention
- **Input Sanitization** — CLI menus hardened against buffer overflows and invalid types

</td>
<td width="50%">

### 💸 Financial Engine
- Automated **due date tracking** and **dynamic fine calculation**
- **User wallet/fund management** system
- Atomic financial transactions with rollback safety

</td>
</tr>
<tr>
<td width="50%">

### 📅 Temporal Logic
- Custom `<ctime>` wrappers handling **Leap Years**
- **Daylight Saving Time** normalizations
- **Month-length variation** edge cases solved

</td>
<td width="50%">

### 📄 Reporting & Data
- **Automated PDF Reports** via `wkhtmltopdf` engine
- On-demand borrowing history & audit trail generation
- **ACID-compliant** SQLite3 persistence layer

</td>
</tr>
</table>

---

## 🏗️ System Architecture

The system is organized around a strict **N-Tier Layered Architecture** following the Single Responsibility Principle:

```
CPP-library-resource-management-system/
│
├── 📄 CMakeLists.txt                  # Cross-platform build configuration
│
├── 📁 src/                            # Core Application Source
│   ├── 🚀 main.cpp                    # Entry point & Dependency Injection container
│   │
│   ├── 📁 domain/                     # ┌─ DOMAIN LAYER
│   │   └── ...                        # │  Core entities: User, Book, Fine, etc.
│   │
│   ├── 📁 infrastructure/             # ├─ INFRASTRUCTURE LAYER
│   │   └── ...                        # │  SQLite DB initializer & Repositories
│   │
│   ├── 📁 services/                   # ├─ SERVICE LAYER
│   │   └── ...                        # │  Business logic & transaction processing
│   │
│   ├── 📁 presentation/               # ├─ PRESENTATION LAYER
│   │   └── ...                        # │  Interactive CLI menus & session mgmt
│   │
│   ├── 📁 validation/                 # ├─ VALIDATION LAYER
│   │   └── ...                        # │  Regex & input sanitization
│   │
│   ├── 📁 PDFGenerator/               # ├─ REPORTING ENGINE
│   │   └── ...                        # │  HTML → PDF conversion pipeline
│   │
│   └── 📁 Utility/                    # └─ UTILITIES
│       └── ...                        #    Date/time parsers & string helpers
│
├── 📁 include/                        # External dependencies (sqlite3.h)
└── 📁 tools/                          # Packaged reporting binaries (wkhtmltox)
```

### Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                        │
│              CLI Menus · Session Management                  │
└─────────────────────────┬───────────────────────────────────┘
                          │ calls
┌─────────────────────────▼───────────────────────────────────┐
│                     SERVICE LAYER                            │
│          Business Logic · Transaction Processing             │
└──────────┬──────────────────────────────┬───────────────────┘
           │ reads/writes                 │ uses
┌──────────▼───────────────┐   ┌──────────▼───────────────────┐
│    REPOSITORY LAYER       │   │       DOMAIN LAYER           │
│  SQLite3 · Parameterized  │   │  Entities · Value Objects    │
│       SQL Queries         │   │  Aggregates · Domain Logic   │
└──────────────────────────┘   └──────────────────────────────┘
```

---

## 🛠️ Tech Stack

| Component | Technology | Version |
|-----------|-----------|---------|
| **Core Language** | C++ | Standard 17 |
| **Build System** | CMake | v3.16+ |
| **Database** | SQLite3 (Amalgamation) | Embedded |
| **Reporting Engine** | wkhtmltopdf | Pre-compiled |
| **Architecture** | N-Tier + Repository Pattern | — |
| **Design Principles** | DDD, OOP, SOLID | — |

---

## 🚀 Getting Started

### Prerequisites

Before building, ensure you have the following installed:

```bash
# Check C++ compiler (GCC/G++ / MinGW-w64 on Windows / MSVC)
g++ --version        # or cl --version for MSVC

# Check CMake
cmake --version      # Requires 3.16+

# Check Git
git --version
```

### Build & Run

```bash
# 1️⃣  Clone the repository
git clone https://github.com/alifaizyab8/CPP-library-resource-management-system.git
cd CPP-library-resource-management-system

# 2️⃣  Generate the build environment
mkdir build
cd build
cmake ..

# 3️⃣  Compile the application
cmake --build .

# 4️⃣  Launch the system
# ── Windows (PowerShell / CMD) ──
.\LibraryManagementSystem.exe

# ── Linux / macOS ──
./LibraryManagementSystem
```

> 💡 **Note:** The `library.db` file will automatically synchronize based on your build path parameters.

---

## 🧩 Design Principles

This project was engineered with the following software engineering principles at its core:

- **`S`** ingle Responsibility — Each class owns exactly one concern
- **`O`** pen/Closed — Entities open for extension, closed for modification
- **`L`** iskov Substitution — Subtypes are substitutable for their base types
- **`I`** nterface Segregation — Lean, focused interfaces throughout
- **`D`** ependency Inversion — High-level modules depend on abstractions
- **Domain-Driven Design** — Business logic lives in the domain, not the database
- **Repository Pattern** — Data access fully decoupled from business logic
- **Dependency Injection** — Dependencies wired at `main.cpp` entry point

---

## 👥 Core Development Team

This system was engineered collaboratively by four developers, each owning a critical vertical:

<table>
<tr>
<td align="center" width="25%">

**Ali Faizyab Khan**
Role will be added soon

</td>
<td align="center" width="25%">

**Syed Waqar Wasif**
Role will be added soon

</td>
<td align="center" width="25%">

**Rameen Zulfi**
Role will be added soon

</td>
<td align="center" width="25%">

**Syeda Bareerah**
Role will be added soon

</td>
</tr>
</table>

---

## 📄 License

Distributed under the **MIT License**. See [`LICENSE`](LICENSE) for full details.

---

<div align="center">

**Built with ❤️ by the LRMS Team · C++17 · SQLite3 · CMake**

*Enterprise-grade library management, engineered from the ground up.*

</div>