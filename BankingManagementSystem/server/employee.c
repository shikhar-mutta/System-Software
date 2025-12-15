/*
 * server/employee.c
 * ------------------------------------------------------------
 * Banking Management System - Employee Module (Improved 2025)
 * Author: ChatGPT
 *
 * Provides:
 *  - Add / Modify Customer Accounts
 *  - View / Process Loans
 *  - View Customer Transactions
 *  - Change Password / Logout
 *
 * Optimized for reliability, safety, and clean communication.
 */

#include "../include/employee.h"
#include "../include/session.h"
#include "../include/structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>

// =========================================================
// 📂 File Paths
// =========================================================
#define CUSTOMER_FILE "../data/customers.txt"
#define LOAN_FILE "../data/loans.txt"
#define TRANSACTION_FILE "../data/transactions.txt"
#define EMPLOYEE_FILE "../data/employees.txt"

// =========================================================
// 🔧 Helper Utilities
// =========================================================

// Trim spaces, tabs, and newlines
static void trim(char *s)
{
    if (!s)
        return;
    size_t len = strlen(s);
    while (len && (s[len - 1] == ' ' || s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == '\t'))
        s[--len] = '\0';
    while (*s == ' ' || *s == '\n' || *s == '\r' || *s == '\t')
        memmove(s, s + 1, strlen(s));
}

// Safe read line from socket
static int read_line(int fd, char *buf, size_t maxlen)
{
    size_t total = 0;
    while (total < maxlen - 1)
    {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0)
            return -1;
        if (c == '\n' || c == '\r')
            break;
        buf[total++] = c;
    }
    buf[total] = '\0';
    return (int)total;
}

// File lock helper
static int lock_file(int fd, short type)
{
    struct flock fl = {0};
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    return fcntl(fd, F_SETLKW, &fl);
}

// Safe send wrapper
static void safe_send(int fd, const char *msg)
{
    if (msg)
        send(fd, msg, strlen(msg), 0);
}

// UI sync helper (avoid output merge)
static inline void sync_delay()
{
    usleep(10000);
}

// =========================================================
// 🔑 Modify Customer Password (by Customer ID - Admin Action)
// =========================================================
int modifyCustomerPasswordById(int cust_id, const char *newpass)
{
    if (cust_id <= 0 || !newpass || strlen(newpass) == 0)
        return 0;

    FILE *fp = fopen(CUSTOMER_FILE, "r+");
    if (!fp)
        return 0;

    int id;
    char uname[64], pass[64], status[16];
    float bal;
    char line[256];
    long pos;
    int updated = 0;

    // Read each line, remember its start position
    while ((pos = ftell(fp)), fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%d %63s %63s %f %15s", &id, uname, pass, &bal, status) == 5)
        {
            if (id == cust_id)
            {
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%d %s %s %.2f %s\n", id, uname, newpass, bal, status);
                fflush(fp);
                updated = 1;
                break;
            }
        }
    }

    fclose(fp);
    return updated;
}

// =========================================================
// 🚦 Modify Customer Account Status (active/inactive)
// =========================================================
int modifyCustomerStatus(const char *uname, const char *newstatus)
{
    if (!uname || !newstatus || strlen(uname) == 0 || strlen(newstatus) == 0)
        return 0;

    FILE *fp = fopen(CUSTOMER_FILE, "r+");
    if (!fp)
        return 0;

    int id;
    char user[64], pass[64], status[16];
    float bal;
    char line[256];
    long pos;
    int updated = 0;

    while ((pos = ftell(fp)), fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%d %63s %63s %f %15s", &id, user, pass, &bal, status) == 5)
        {
            if (strcmp(user, uname) == 0)
            {
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%d %s %s %.2f %s\n", id, user, pass, bal, newstatus);
                fflush(fp);
                updated = 1;
                break;
            }
        }
    }

    fclose(fp);
    return updated;
}

// =========================================================
// 🔐 Change Employee Password by ID
// =========================================================
int changeEmployeePassword(int employeeId, const char *oldPass, const char *newPass)
{
    if (employeeId <= 0 || !oldPass || !newPass)
        return 0;

    FILE *fp = fopen(EMPLOYEE_FILE, "r");
    if (!fp)
        return 0;

    typedef struct
    {
        int id;
        char uname[64];
        char pass[64];
        char role[32];
    } Emp;

    Emp employees[500];
    int count = 0;
    int changed = 0;

    // Read all employees
    while (count < 500 && fscanf(fp, "%d %63s %63s %31s",
                                 &employees[count].id,
                                 employees[count].uname,
                                 employees[count].pass,
                                 employees[count].role) == 4)
    {
        if (employees[count].id == employeeId && 
            strcmp(employees[count].pass, oldPass) == 0)
        {
            // Update password
            strncpy(employees[count].pass, newPass, sizeof(employees[count].pass) - 1);
            employees[count].pass[sizeof(employees[count].pass) - 1] = '\0';
            changed = 1;
        }
        count++;
    }
    fclose(fp);

    if (!changed)
        return 0;

    // Write all employees back with proper formatting (same format as addEmployee)
    fp = fopen(EMPLOYEE_FILE, "w");
    if (!fp)
        return 0;

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %s %s\n",
                employees[i].id,
                employees[i].uname,
                employees[i].pass,
                employees[i].role);
    }
    fclose(fp);

    return changed;
}

// =========================================================
// 👤 Add New Customer (auto-increment 4-digit ID)
// =========================================================
int addCustomer(const char *uname, const char *pass, float initial_balance)
{
    if (!uname || !pass || strlen(uname) == 0 || strlen(pass) == 0)
        return 0;

    FILE *fp = fopen(CUSTOMER_FILE, "a+"); // open for both read/write
    if (!fp)
        return 0;

    int fd = fileno(fp);
    if (lock_file(fd, F_WRLCK) == -1)
    {
        fclose(fp);
        return 0;
    }

    rewind(fp); // make sure we start from the beginning

    int lastId = 1000; // start before first valid ID
    int id;
    char name[64], password[64], status[16];
    float balance;

    // Read all existing customers to find last ID
    while (fscanf(fp, "%d %63s %63s %f %15s", &id, name, password, &balance, status) == 5)
    {
        if (id > lastId)
            lastId = id;
    }

    int newId = lastId + 1;

    // Append new customer
    fprintf(fp, "%d %s %s %.2f active\n", newId, uname, pass, initial_balance);
    fflush(fp);
    fsync(fd);

    lock_file(fd, F_UNLCK);
    fclose(fp);

    printf("✅ New customer added with ID: %d\n", newId);

    return newId; // return the ID assigned (instead of 1/0)
}

// =========================================================
// 💰 Modify Customer Balance (by customer ID)
// =========================================================
int modifyCustomerById(int cust_id, float new_balance)
{
    if (cust_id <= 0)
        return 0;

    FILE *fp = fopen(CUSTOMER_FILE, "r+");
    if (!fp)
        return 0;

    int id;
    char uname[64], pass[64], status[16];
    float bal;
    char line[256];
    long pos;
    int updated = 0;

    // Loop through file line by line
    while ((pos = ftell(fp)), fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%d %63s %63s %f %15s", &id, uname, pass, &bal, status) == 5)
        {
            if (id == cust_id)
            {
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%d %s %s %.2f %s\n", id, uname, pass, new_balance, status);
                fflush(fp);
                updated = 1;
                break;
            }
        }
    }

    fclose(fp);
    return updated;
}

/// =========================================================
// 🧾 Show Loan Applications Assigned to This Employee
// =========================================================
void showEmployeeLoans(int client_fd, int emp_id)
{
    FILE *fp = fopen(LOAN_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No loan records found.\n");
        return;
    }

    char line[512], output[8192];
    memset(output, 0, sizeof(output));

    strcat(output, "\n📄 Loan Applications Assigned to You\n");
    strcat(output, "====================================================\n");

    int found = 0;
    while (fgets(line, sizeof(line), fp))
    {
        char loanId[30], custName[50], status[20], reason[100];
        int custId, assignedEmpId, approvedEmpId;
        float amount;

        if (sscanf(line, "%29s %d %49s %f %19s %99s %d %d",
                   loanId, &custId, custName, &amount,
                   status, reason, &assignedEmpId, &approvedEmpId) == 8)
        {
            if (assignedEmpId == emp_id)
            {
                found = 1;
                char entry[512];
                snprintf(entry, sizeof(entry),
                         "🏦 Loan ID: %s\n👤 Customer: %s (ID: %d)\n💰 Amount: ₹%.2f\n📊 Status: %s\n📝 Reason: %s\n✅ Approved Employee ID: %04d\n----------------------------------------------------\n",
                         loanId, custName, custId, amount, status, reason, approvedEmpId);

                if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                    strcat(output, entry);
            }
        }
    }

    fclose(fp);

    if (!found)
        strcat(output, "⚠️ No loans are currently assigned to you.\n");
    else
        strcat(output, "====================================================\n🏁 End of Assigned Loans.\n\n");

    safe_send(client_fd, output);
    usleep(50000);
}

// =========================================================
// 🏦 Process Loan (Approve/Reject) and Perform Transaction
// =========================================================
int processLoan(const char *loan_id, const char *decision, int employee_id)
{
    if (!loan_id || !decision || employee_id <= 0)
        return 0;

    FILE *fp = fopen(LOAN_FILE, "r+");
    if (!fp)
        return 0;

    char line[512];
    char loanId[30], custName[50], status[20], reason[100];
    int custId, assignedEmpId, approvedEmpId;
    float amount;
    long pos;
    int updated = 0;

    // --- Step 1: Find loan by ID ---
    while ((pos = ftell(fp)), fgets(line, sizeof(line), fp))
    {
        int parsed = sscanf(line, "%s %d %s %f %s %s %d %d",
                            loanId, &custId, custName, &amount,
                            status, reason, &assignedEmpId, &approvedEmpId);

        if (parsed == 8 && strcmp(loanId, loan_id) == 0)
        {
            // 🧠 Verify assignment before allowing approval
            // Accept "approved", "a", or "A" as approved
            if (strcasecmp(decision, "approved") == 0 || 
                strcasecmp(decision, "a") == 0)
            {
                if (assignedEmpId != employee_id)
                {
                    fclose(fp);
                    return -1; // ❌ Not assigned to this employee
                }

                approvedEmpId = employee_id;
                strcpy(status, "approved");

                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%s %04d %s %.2f %s %s %04d %04d\n",
                        loanId, custId, custName, amount,
                        status, reason, assignedEmpId, approvedEmpId);
                fflush(fp);

                updated = 1;

                // --- Step 2: Update customer balance ---
                FILE *cust_fp = fopen(CUSTOMER_FILE, "r+");
                if (cust_fp)
                {
                    char c_line[256];
                    int id;
                    char uname[64], pass[64], cstatus[16];
                    float bal;
                    long cpos;

                    while ((cpos = ftell(cust_fp)), fgets(c_line, sizeof(c_line), cust_fp))
                    {
                        if (sscanf(c_line, "%d %63s %63s %f %15s",
                                   &id, uname, pass, &bal, cstatus) == 5)
                        {
                            if (id == custId)
                            {
                                bal += amount;
                                fseek(cust_fp, cpos, SEEK_SET);
                                fprintf(cust_fp, "%d %s %s %.2f %s\n",
                                        id, uname, pass, bal, cstatus);
                                fflush(cust_fp);

                                // --- Step 3: Log transaction ---
                                FILE *txn_fp = fopen(TRANSACTION_FILE, "a");
                                if (txn_fp)
                                {
                                    char datetime[64];
                                    time_t t = time(NULL);
                                    struct tm *tm_info = localtime(&t);
                                    strftime(datetime, sizeof(datetime), "%Y-%m-%d_%H:%M:%S", tm_info);

                                    fprintf(txn_fp, "TXN_%ld %s loan_credit %.2f %s %.2f %d\n",
                                            t, uname, amount, datetime, bal, custId);
                                    fclose(txn_fp);
                                }
                                break;
                            }
                        }
                    }
                    fclose(cust_fp);
                }
            }
            else // rejected
            {
                strcpy(status, "rejected");
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%s %04d %s %.2f %s %s %04d %04d\n",
                        loanId, custId, custName, amount,
                        status, reason, assignedEmpId, approvedEmpId);
                fflush(fp);
                updated = 1;
            }

            break;
        }
    }

    fclose(fp);
    return updated;
}

// =========================================================
// 💳 View Customer Transactions
// =========================================================
void viewCustomerTransactions(int client_fd, int customerId)
{
    FILE *fp = fopen(TRANSACTION_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No transaction history available.\n");
        return;
    }

    char line[512], output[8192];
    memset(output, 0, sizeof(output));

    strcat(output, "\n📜 Customer Transaction History\n");
    strcat(output, "====================================================\n");

    int found = 0;

    while (fgets(line, sizeof(line), fp))
    {
        char txnId[32], username[64], type[32], datetime[64];
        float amount, totalBal;
        int custId;

        // Format: TXN_ID username type amount datetime balance custId
        if (sscanf(line, "%31s %63s %31s %f %63s %f %d",
                   txnId, username, type, &amount, datetime, &totalBal, &custId) == 7)
        {
            if (custId == customerId)
            {
                found = 1;
                char entry[512];
                snprintf(entry, sizeof(entry),
                         "🧾 Transaction ID: %s\n"
                         "📂 Type: %s\n"
                         "💰 Amount: ₹%.2f\n"
                         "🕓 Date: %s\n"
                         "🏦 Balance After: ₹%.2f\n"
                         "----------------------------------------------------\n",
                         txnId, type, amount, datetime, totalBal);

                if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                    strcat(output, entry);
            }
        }
    }

    fclose(fp);

    if (!found)
        strcat(output, "⚠️ No transactions found for this customer.\n");
    else
        strcat(output, "====================================================\n🏁 End of Transaction History.\n\n");

    safe_send(client_fd, output);
    usleep(50000);
}

// =========================================================
// 🧑‍💼 Employee Dashboard Menu
// =========================================================
void handleEmployeeMenu(int client_fd, const int userId, const char *username)
{
    char buf[MAX], msg[1024];

    while (1)
    {
        snprintf(msg, sizeof(msg),
                 "\n=== 🧑‍💼 Welcome, %s (Employee) ===\n"
                 "--------------------------------------------\n"
                 "1. addcust         - Add New Customer\n"
                 "2. modifycustbal   - Modify Customer Balance\n"
                 "3. modifycustpass  - Reset Customer Password\n"
                 "4. loans           - View All Loan Applications\n"
                 "5. processloan     - Approve / Reject Loan\n"
                 "6. viewtrans       - View Customer Transactions\n"
                 "7. changepass      - Change Password\n"
                 "8. logout          - Logout\n"
                 "9. exit            - Exit\n"
                 "--------------------------------------------\n"
                 "Enter command: ",
                 username);
        safe_send(client_fd, msg);

        memset(buf, 0, sizeof(buf));
        if (read_line(client_fd, buf, sizeof(buf)) <= 0)
        {
            printf("❌ %s disconnected.\n", username);
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            close(client_fd);
            return;
        }
        trim(buf);

        // ======= COMMAND HANDLERS =======

        // 1️ Add Customer
        if (!strcmp(buf, "1") || !strcmp(buf, "addcust"))
        {
            safe_send(client_fd, "Enter new customer username: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char uname[64];
            strcpy(uname, buf);

            safe_send(client_fd, "Enter password for customer: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char pass[64];
            strcpy(pass, buf);

            safe_send(client_fd, "Enter initial balance: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            float bal = atof(buf);

            int newCustomerId = addCustomer(uname, pass, bal);
            if (newCustomerId > 0)
            {
                char successMsg[256];
                snprintf(successMsg, sizeof(successMsg),
                         "✅ Customer added successfully.\n"
                         "   Customer ID: %d\n"
                         "   Username: %s\n"
                         "   Initial Balance: ₹%.2f\n",
                         newCustomerId, uname, bal);
                safe_send(client_fd, successMsg);
            }
            else
            {
                safe_send(client_fd, "❌ Failed to add customer.\n");
            }
        }

        // 2 Modify Customer (by ID)
        else if (!strcmp(buf, "2") || !strcmp(buf, "modifycustbal"))
        {
            safe_send(client_fd, "Enter customer ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int cust_id = atoi(buf);
            if (cust_id <= 0)
            {
                safe_send(client_fd, "❌ Invalid ID entered.\n");
                return;
            }

            safe_send(client_fd, "Enter new balance: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            float bal = atof(buf);
            if (bal < 0)
            {
                safe_send(client_fd, "❌ Balance cannot be negative.\n");
                return;
            }
            int result = modifyCustomerById(cust_id, bal);
            if (result)
                safe_send(client_fd, "✅ Customer balance updated successfully.\n");
            else
                safe_send(client_fd, "❌ Customer ID not found.\n");
        }

        // 3 Modify Customer Password (by ID)
        else if (!strcmp(buf, "3") || !strcmp(buf, "modifycustpass"))
        {
            safe_send(client_fd, "Enter customer ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int cust_id = atoi(buf);
            if (cust_id <= 0)
            {
                safe_send(client_fd, "❌ Invalid customer ID entered.\n");
                return;
            }

            safe_send(client_fd, "Enter new password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            char newpass[64];
            strncpy(newpass, buf, sizeof(newpass) - 1);
            newpass[sizeof(newpass) - 1] = '\0';

            if (strlen(newpass) == 0)
            {
                safe_send(client_fd, "❌ Password cannot be empty.\n");
                return;
            }

            int result = modifyCustomerPasswordById(cust_id, newpass);

            if (result)
                safe_send(client_fd, "✅ Customer password updated successfully.\n");
            else
                safe_send(client_fd, "❌ Customer ID not found or failed to update.\n");
        }

        // 4 View Loans
        else if (!strcmp(buf, "4") || !strcmp(buf, "loans"))
        {
            int emp_id = userId; // convert logged-in employee's ID to integer

            // For employees: show only assigned loans
            showEmployeeLoans(client_fd, emp_id);
        }

        // 5 Process Loan
        else if (!strcmp(buf, "5") || !strcmp(buf, "processloan"))
        {
            safe_send(client_fd, "Enter Loan ID to process: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char loanID[64];
            strcpy(loanID, buf);

            safe_send(client_fd, "Enter decision (approved/rejected): ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int result = processLoan(loanID, buf, userId);

            if (result)
            {
                safe_send(client_fd, "✅ Loan processed successfully.\n");
            }
            else
            {
                safe_send(client_fd, "❌ Loan ID not found or failed to process.\n");
            }
        }

        // 6 View Transactions
        else if (!strcmp(buf, "6") || !strcmp(buf, "viewtrans"))
        {
            safe_send(client_fd, "Enter Customer ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int custId = atoi(buf);
            if (custId <= 0)
            {
                safe_send(client_fd, "❌ Invalid Customer ID.\n");
            }
            else
            {
                viewCustomerTransactions(client_fd, custId);
            }
        }

        // 7 Change Password
        else if (!strcmp(buf, "7") || !strcmp(buf, "changepass"))
        {
            safe_send(client_fd, "Enter old password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char oldPass[64];
            strcpy(oldPass, buf);

            safe_send(client_fd, "Enter new password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char newPass[64];
            strcpy(newPass, buf);

            if (changeEmployeePassword(userId, oldPass, newPass))
                safe_send(client_fd, "✅ Password changed successfully.\n");
            else
                safe_send(client_fd, "❌ Incorrect old password.\n");
        }

        // 8 Logout
        else if (!strcmp(buf, "8") || !strcmp(buf, "logout"))
        {
            safe_send(client_fd, "👋 Logged out successfully. Returning to login page...\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        }

        // 9 Exit
        else if (!strcmp(buf, "9") || !strcmp(buf, "exit"))
        {
            safe_send(client_fd, "👋 Exiting program.\n 👋Good bye!\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            close(client_fd);
            exit(0);
        }

        // ❌ Invalid
        else
            safe_send(client_fd, "❌ Invalid command. Try again.\n");
    }
}
