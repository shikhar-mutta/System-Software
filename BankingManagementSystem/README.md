# 🏦 Banking Management System

**Course:** CS-513 System Software – Mini Project  
**Institute:** International Institute of Information Technology, Bangalore  
**Submitted by:** MT2025114 – Shikhar Bhadreshkumar Mutta  
**Submission Date:** 31st October 2025  

---

## 📘 Project Overview

The **Banking Management System (BMS)** simulates the fundamental operations of a real-world bank using **C programming**, **system calls**, and **socket programming**. It demonstrates **client-server architecture**, **process synchronization**, **file-based database management**, and **secure concurrent access** to shared resources.

This system supports **multiple user roles** — *Customer*, *Employee*, *Manager*, and *Administrator* — each with distinct access privileges and functionalities.

---

## 🧩 Key Objectives

- Implement a multi-user, role-based banking system.
- Use **system-level programming** (no external DBMS).
- Handle **concurrent access** with file locking and synchronization.
- Maintain **ACID properties** for all transactions.
- Simulate **real-world banking operations** like deposits, withdrawals, transfers, and loan processing.

---

## 🧠 System Architecture

The project is structured as a **client-server model**:

```
┌──────────────────┐        ┌────────────────────┐
│     CLIENTS      │◄──────►│      SERVER        │
│ (Customers, etc.)│        │ Handles all logic  │
└──────────────────┘        └────────────────────┘
```

Each client connects to the central server via **sockets**, and all data is persisted in files under the `/data` directory. Synchronization and file locks prevent race conditions during concurrent transactions.

---

## 🧑‍💼 User Roles and Functionalities

### 1. 👤 **Customer**
- Secure login (one session per user)
- View account balance
- Deposit / Withdraw money
- Transfer funds
- Apply for loans
- View transaction history
- Change password
- Submit feedback
- Logout and exit

### 2. 🧾 **Bank Employee**
- Login (single session)
- Add new customer
- Modify existing customer details
- Process and approve/reject loan applications
- View assigned loan applications
- View customer transaction logs (passbook)
- Change password
- Logout and exit

### 3. 🧑‍💼 **Manager**
- Login (single session)
- Activate or deactivate customer accounts
- Assign loan applications to employees
- Review customer feedback
- Change password
- Logout and exit

### 4. 👨‍💻 **Administrator**
- Login (single session)
- Add new bank employees
- Modify customer or employee details
- Manage user roles and access permissions
- Change password
- Logout and exit

---

## ⚙️ Technical Specifications

| Category | Details |
|-----------|----------|
| **Language** | C |
| **Paradigm** | Procedural, Modular |
| **Concurrency** | File locking, semaphores, and synchronization |
| **Architecture** | Multi-client server using sockets |
| **Storage** | File-based database (no SQL) |
| **Security** | Role-based authentication, password protection |
| **Execution** | Linux/UNIX environment using system calls |
| **Make System** | `make` utility to build server and client executables |

---

## 📁 Project Structure

```
BankingManagementSystem/
├── client/
│   ├── admin.c
│   ├── customer.c
│   ├── employee.c
│   ├── manager.c
│   ├── client.c
│   ├── Makefile
├── server/
│   ├── server.c
│   ├── Makefile
├── data/
│   ├── customers.dat
│   ├── employees.dat
│   ├── transactions.dat
│   ├── loans.dat
├── Makefile
└── README.md
```

---

## 🏗️ Build and Run Instructions

### 1. **Compile the Project**
```bash
make
```
This builds both the **server** and **client** components automatically.

### 2. **Run the Server**
```bash
./server/server
```

### 3. **Run the Client**
Open another terminal and run:
```bash
./client/client
```

### 4. **Interact with the System**
Login using predefined credentials for each role to perform corresponding operations.

---

## 🔐 Concurrency and Synchronization

- All transaction operations (deposit, withdraw, transfer) use **file locks** (`fcntl`) to avoid race conditions.
- **Semaphores** and **mutexes** ensure safe concurrent access by multiple clients.
- **Atomic operations** guarantee the **ACID** properties for financial transactions.

---

## 🧱 Implementation Highlights

- **Socket programming:** Enables client-server communication.
- **System calls:** Used for file operations (`open`, `read`, `write`, `lseek`), locking, and process handling.
- **Error handling:** All critical operations include error checks.
- **Role-based access control:** Implemented using structured login and verification.
- **File-based persistence:** Data is stored in binary files for efficiency.

---

## 📊 Evaluation Criteria Mapping

| Criterion | Description |
|------------|-------------|
| **Blueprint/Class Diagram** | Includes class, component, and sequence diagrams (provided separately). |
| **Working Code** | Implements all required modules and operations. |
| **Concurrency & Synchronization** | Demonstrated using file locks and semaphores. |
| **ACID Properties** | Ensured in all transaction-based operations. |

---

## 📐 UML Diagrams (Provided Separately)

- `Class Diagram`: Defines entities and their relationships.  
- `Component Diagram`: Depicts client-server and role modules.  
- `Sequence Diagram`: Demonstrates transaction flow (e.g., fund transfer).  

*(See `/docs/diagrams/` or refer to attached `.md` Mermaid scripts.)*

---

## 🧪 Sample Test Flow

1. Start server and connect multiple clients.  
2. Login as `Customer` and perform deposit or transfer operations.  
3. Concurrently login as `Employee` to process loan requests.  
4. Manager activates/deactivates accounts.  
5. Administrator adds/modifies users.  
6. Verify data consistency and synchronization across all sessions.

---

## 🚀 Future Enhancements

- Migrate from file-based to **database-backed** architecture (MySQL/PostgreSQL).  
- Implement **web-based front-end** using REST APIs.  
- Add **two-factor authentication** for enhanced security.  
- Introduce **data encryption** for stored passwords and transaction logs.  

---

## 🧑‍🏫 Acknowledgment

This project was developed as part of the **CS-513 (System Software)** course under the guidance of the **IIIT Bangalore faculty**.  
Special thanks to the teaching staff for their support and evaluation guidelines.

---

## 📜 License

This project is developed for academic purposes under the **IIIT Bangalore** course curriculum. Redistribution without permission is prohibited.

---

> 💡 *“Concurrency is not parallelism — it’s managing simultaneous processes safely. This project demonstrates both.”*
