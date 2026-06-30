# Low-Level Design (LLD) in C++

Welcome to my personal repository of Low-Level Design (LLD) implementations in C++. This repository is dedicated to practicing object-oriented analysis and design, applying design patterns, and building simple, modular, and thread-safe systems.

## 🚀 Overview

This repository contains C++ implementations for several classic low-level design problems. Each system is designed with standard object-oriented programming (OOP) principles, structured class hierarchies, clean code standards, and basic concurrency controls (where applicable).

## 📁 Repository Contents

Here is a summary of the LLD problems currently included:

| Problem / System | Source File | Core Concepts & Design Features |
| :--- | :--- | :--- |
| **☕ Coffee Vending Machine** | [`Coffee_Vending_Machine.cpp`](./Coffee_Vending_Machine.cpp) | Recipe & Ingredient Management, Menu Display, Payment Handling, Concurrency & Thread-safety (`std::mutex`), Low Inventory Alerts. |
| **📝 Logging Framework** | [`Logging_Framework.cpp`](./Logging_Framework.cpp) | Multiple log levels (INFO, DEBUG, ERROR), Chain of Responsibility / Observer pattern representation, thread-safety, and log targets (Console, File). |
| **🚗 Parking Lot** | [`Parking_Lot.cpp`](./Parking_Lot.cpp) | Multi-level parking, multiple vehicle types (Car, Bike, Truck), spot assignment strategies, ticketing & payment system. |
| **📚 Stack Overflow** | [`StackOverflow.cpp`](./StackOverflow.cpp) | Users, Questions, Answers, Comments, Tag Management, voting system rules (preventing self-voting/duplicate voting), reputation score calculation. |
| **📅 Task Management System** | [`Task_Management_System.cpp`](./Task_Management_System.cpp) | Task lifecycle management, priority/status tracking, history logs, search/filters, and user assignments. |
| **🚦 Traffic Signal Control** | [`Traffic_Signal_Control_System.cpp`](./Traffic_Signal_Control_System.cpp) | Traffic light states, scheduling algorithms (dynamic adjustments based on traffic density), and signal synchronization. |
| **🥤 Vending Machine** | [`Vending_Machine.cpp`](./Vending_Machine.cpp) | State Pattern implementation (NoMoneyState, HasMoneyState, DispenseState), product inventory, coin/note validation, returning change. |
| **🏧 ATM System** | [`ATM_System.cpp`](./ATM_System.cpp) | Balance inquiry, cash withdrawal/deposit, user authentication (card/PIN validation), cash dispenser state, account models (WIP). |

---

## 🛠️ How to Compile and Run

You can compile any of the source files using a modern C++ compiler (supporting C++17 or later).

For example, to compile and run the **Coffee Vending Machine**:

```bash
# Compile
g++ -std=c++17 Coffee_Vending_Machine.cpp -o Coffee_Vending_Machine

# Run
./Coffee_Vending_Machine
```

## 🧠 Design Principles Applied

- **SOLID Principles**: Focused on Single Responsibility, Open-Closed design, and Interface Segregation.
- **State Pattern**: Used in Vending Machine to handle state transitions elegantly.
- **Singleton & Thread-safety**: Used in shared resource managers, using `std::mutex` and `std::lock_guard` for concurrent safety.
- **Factory & Strategy Patterns**: Used for clean object instantiation and pluggable algorithms (e.g., parking spot assignment, payment strategies).

---
*This repository is built and maintained for personal practice and preparation for LLD interviews.*
