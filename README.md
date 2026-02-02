# 📦 Warehouse Management System

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Build](https://img.shields.io/badge/build-passing-brightgreen?style=for-the-badge)
![Course](https://img.shields.io/badge/Systems_Programming-SPL-orange?style=for-the-badge)
![University](https://img.shields.io/badge/Ben_Gurion_University-BGU-blue?style=for-the-badge)

A comprehensive **C++ simulation** of a warehouse management system.
This project implements a robust backend for managing inventory, volunteers, customers, and orders using **Object-Oriented Programming** principles and strict **Memory Management** (Rule of 5).

---

## 🏗️ Architecture & Design

The system is built using the **Command Design Pattern**, where every user interaction is encapsulated as an `Action` object.

```mermaid
classDiagram
    class WareHouse {
        -vector~Volunteer*~ volunteers
        -vector~Order*~ pendingOrders
        -vector~Customer*~ customers
        -vector~BaseAction*~ actionsLog
        +start()
    }
    class BaseAction {
        <<abstract>>
        +act(WareHouse& wareHouse)
    }
    class Volunteer {
        <<abstract>>
        +canTakeOrder(Order& order)
        +acceptOrder(Order& order)
    }
    
    WareHouse "1" *-- "*" Volunteer : manages
    WareHouse "1" *-- "*" Customer : serves
    BaseAction ..> WareHouse : modifies
    
    Volunteer <|-- CollectorVolunteer
    Volunteer <|-- DriverVolunteer


**## Key Features**
**Polymorphism**: Diverse types of Volunteers (Collectors, Drivers) and Customers (Soldiers, Civilians) with unique behaviors.

**Memory Safety**: Full implementation of the Rule of 5 (Destructor, Copy Constructor, Copy Assignment, Move Constructor, Move Assignment) to prevent memory leaks.

**Command Pattern**: User commands (step, order, status) are parsed and executed as polymorphic objects inheriting from BaseAction.

👤 Author
Yuval and Michal - Computer Science Students, BGU
