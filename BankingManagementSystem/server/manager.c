/*
 * server/manager.c
 * ------------------------------------------------------------
 * Banking Management System - Manager Module (2025)
 * Author: ChatGPT
 *
 * Provides Manager Operations:
 *  - Activate/Deactivate Customer Accounts
 *  - View Loan Applications
 *  - Assign Loans to Employees
 *  - View Customer Feedback
 *  - Change Password
 *  - Logout / Exit
 */

#include "../include/manager.h"
#include "../include/session.h"
#include "../include/structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <errno.h>

// =========================================================
// 📂 File Paths
// =========================================================
#define CUSTOMER_FILE "../data/customers.txt"
#define LOAN_FILE "../data/loans.txt"
#define FEEDBACK_FILE "../data/feedback.txt"
#define EMPLOYEE_FILE "../data/employees.txt"

// =========================================================
// 🧰 Utility Helpers
// =========================================================

// Trim whitespace (space, tab, newline)
static void trim(char *s)
{
    if (!s)
        return;

    char *start = s;
    while (*start && (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n'))
        start++;

    if (start != s)
        memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' ||
                       s[len - 1] == '\r' || s[len - 1] == '\n'))
        s[--len] = '\0';
}

// Safe send wrapper
static void safe_send(int fd, const char *msg)
{
    if (msg && fd >= 0)
    {
        ssize_t sent = send(fd, msg, strlen(msg), 0);
        if (sent < 0 && errno != EPIPE && errno != ECONNRESET)
        {
            // Log error but don't crash - connection might be closed
        }
    }
}

// Read line safely from socket
static int read_line(int fd, char *buf, size_t maxlen)
{
    if (!buf || maxlen == 0 || fd < 0)
        return -1;
        
    size_t total = 0;
    while (total < maxlen - 1)
    {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0)
        {
            if (n == 0)
                return 0; // Connection closed
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                continue; // Retry
            return -1; // Error
        }
        if (c == '\n' || c == '\r')
            break;
        buf[total++] = c;
    }
    buf[total] = '\0';
    return (int)total;
}

// Safe bounded string copy
static void safe_copy(char *dest, const char *src, size_t size)
{
    if (!dest || !src || size == 0)
        return;
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

// =========================================================
// 🔐 Change Manager Password Securely (by ID)
// =========================================================
int changeManagerPassword(int userId, const char *oldPass, const char *newPass)
{
    if (userId <= 0 || !oldPass || !newPass || strlen(newPass) == 0)
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
        if (employees[count].id == userId && 
            strcmp(employees[count].pass, oldPass) == 0 && 
            strcmp(employees[count].role, "manager") == 0)
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
// ⚙️ Activate / Deactivate Customer Account (by ID)
// =========================================================
int setCustomerStatus(int custId, const char *newStatus, char *outCustomerName, size_t nameLen, char *outStatus, size_t statusLen)
{
    if (custId <= 0 || !newStatus || strlen(newStatus) == 0)
        return 0;

    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp)
        return 0;

    typedef struct
    {
        int id;
        char uname[64];
        char pass[64];
        float bal;
        char status[16];
    } Cust;

    Cust customers[500];
    int count = 0;
    int changed = 0;

    // Read all customers
    while (count < 500 && fscanf(fp, "%d %63s %63s %f %15s",
                                 &customers[count].id,
                                 customers[count].uname,
                                 customers[count].pass,
                                 &customers[count].bal,
                                 customers[count].status) == 5)
    {
        if (customers[count].id == custId)
        {
            // Store customer name and new status for success message
            if (outCustomerName && nameLen > 0)
            {
                strncpy(outCustomerName, customers[count].uname, nameLen - 1);
                outCustomerName[nameLen - 1] = '\0';
            }
            if (outStatus && statusLen > 0)
            {
                strncpy(outStatus, newStatus, statusLen - 1);
                outStatus[statusLen - 1] = '\0';
            }
            // Update status
            strncpy(customers[count].status, newStatus, sizeof(customers[count].status) - 1);
            customers[count].status[sizeof(customers[count].status) - 1] = '\0';
            changed = 1;
        }
        count++;
    }
    fclose(fp);

    if (!changed)
        return 0;

    // Write all customers back with proper formatting (same format as other modules)
    fp = fopen(CUSTOMER_FILE, "w");
    if (!fp)
        return 0;

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %s %.2f %s\n",
                customers[i].id,
                customers[i].uname,
                customers[i].pass,
                customers[i].bal,
                customers[i].status);
    }
    fclose(fp);

    return changed;
}

// =========================================================
// 🧾 View All Loan Applications
// =========================================================
void showAllLoanApplications(int client_fd)
{
    FILE *fp = fopen(LOAN_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No loan applications found.\n");
        return;
    }

    char line[512], output[16384];
    memset(output, 0, sizeof(output));

    strcat(output, "\n📄 All Loan Applications\n");
    strcat(output, "====================================================\n");

    int totalShown = 0;
    while (fgets(line, sizeof(line), fp))
    {
        char loanId[30], custName[50], status[20], reason[100];
        int custId, assignedEmpId, approvedEmpId;
        float amount;

        if (sscanf(line, "%29s %d %49s %f %19s %99s %d %d",
                   loanId, &custId, custName, &amount,
                   status, reason, &assignedEmpId, &approvedEmpId) == 8)
        {
            char entry[512];
            snprintf(entry, sizeof(entry),
                     "🏦 Loan ID: %s\n👤 Customer: %s (ID: %d)\n💰 Amount: ₹%.2f\n📊 Status: %s\n📝 Reason: %s\n👷 Assigned Employee ID: %04d\n✅ Approved Employee ID: %04d\n----------------------------------------------------\n",
                     loanId, custName, custId, amount, status, reason, assignedEmpId, approvedEmpId);
            if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                strcat(output, entry);
            totalShown++;
        }
    }
    fclose(fp);

    if (!totalShown)
        strcat(output, "⚠️ No loan applications found.\n");
    else
        strcat(output, "====================================================\n🏁 End of Loan List.\n\n");

    safe_send(client_fd, output);
    usleep(50000);
}

// =========================================================
// ⚙️ Assign Loan to Employee
// =========================================================
int assignLoanToEmployee(const char *loan_id, int employeeId)
{
    if (!loan_id || employeeId <= 0)
        return 0;

    FILE *empFile = fopen(EMPLOYEE_FILE, "r");
    if (!empFile)
        return 0;

    int id;
    char uname[64], pass[64], role[32];
    int validEmployee = 0;

    while (fscanf(empFile, "%d %63s %63s %31s", &id, uname, pass, role) == 4)
    {
        if (id == employeeId && strcasecmp(role, "employee") == 0)
        {
            validEmployee = 1;
            break;
        }
    }
    fclose(empFile);

    if (!validEmployee)
        return 0;

    FILE *fp = fopen(LOAN_FILE, "r+");
    if (!fp)
        return 0;

    char line[512];
    long posBefore = 0;
    int updated = 0;

    while (fgets(line, sizeof(line), fp))
    {
        posBefore = ftell(fp);
        char loanId[32], custName[64], status[32], reason[128];
        int custId, assignedEmpId, approvedEmpId;
        float amount;

        if (sscanf(line, "%31s %d %63s %f %31s %127s %d %d",
                   loanId, &custId, custName, &amount,
                   status, reason, &assignedEmpId, &approvedEmpId) == 8)
        {
            if (strcmp(loanId, loan_id) == 0)
            {
                fseek(fp, posBefore - strlen(line), SEEK_SET);
                fprintf(fp, "%s %04d %s %.2f %s %s %04d %04d\n",
                        loanId, custId, custName, amount,
                        status, reason, employeeId, approvedEmpId);
                updated = 1;
                break;
            }
        }
    }

    fclose(fp);
    return updated;
}
// =========================================================
// 💬 View Customer Feedback (Optimized send)
// =========================================================
void viewFeedback(int client_fd)
{
    FILE *fp = fopen(FEEDBACK_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No feedback records found.\n");
        return;
    }

    char output[16384]; // 16KB buffer for all feedbacks
    memset(output, 0, sizeof(output));

    strcat(output, "\n💬 Customer Feedback Viewer\n");
    strcat(output, "====================================================\n");

    char line[512];
    int totalShown = 0;

    while (fgets(line, sizeof(line), fp))
    {
        char datetime[64], username[64], message[256];
        int custId;

        if (sscanf(line, "[%63[^]]] %63[^'('](%d): %[^\n]",
                   datetime, username, &custId, message) == 4)
        {
            char entry[512];
            snprintf(entry, sizeof(entry),
                     "🕓 %s\n👤 %s (UserId: %d)\n💬 %s\n----------------------------------------------------\n",
                     datetime, username, custId, message);

            if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                strcat(output, entry);
            else
                break; // prevent overflow
            totalShown++;
        }
    }

    fclose(fp);

    if (totalShown == 0)
        strcat(output, "⚠️ No feedback records found.\n");
    else
        strcat(output, "====================================================\n🏁 End of Feedback List.\n\n");

    safe_send(client_fd, output);
}

// =========================================================
// 🧑‍💼 Manager Dashboard Menu
// =========================================================
void handleManagerMenu(int client_fd, const int userId, const char *username)
{
    char buf[MAX], msg[512];

    while (1)
    {
        snprintf(msg, sizeof(msg),
                 "\n=== 👔 Welcome, %s (Manager) ===\n"
                 "--------------------------------------------\n"
                 "1. setstatus   - Activate/Deactivate Customer\n"
                 "2. loans       - View All Loan Applications\n"
                 "3. assignloan  - Assign Loan to Employee\n"
                 "4. feedback    - View Customer Feedback\n"
                 "5. changepass  - Change Password\n"
                 "6. logout      - Logout\n"
                 "7. exit        - Exit\n"
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

        // === Option 1: Activate/Deactivate Customer ===
        if (!strcmp(buf, "1") || !strcmp(buf, "setstatus"))
        {
            safe_send(client_fd, "Enter Customer ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            int custId = atoi(buf);

            if (custId <= 0)
            {
                safe_send(client_fd, "❌ Invalid Customer ID.\n");
                continue;
            }

            safe_send(client_fd, "Enter new status (active/inactive): ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            // Normalize status input: accept "i"/"inactive" or "a"/"active" (case-insensitive)
            char normalizedStatus[16] = {0};
            if (strcasecmp(buf, "i") == 0 || strcasecmp(buf, "inactive") == 0)
            {
                strcpy(normalizedStatus, "inactive");
            }
            else if (strcasecmp(buf, "a") == 0 || strcasecmp(buf, "active") == 0)
            {
                strcpy(normalizedStatus, "active");
            }
            else
            {
                // If input doesn't match, use as-is but validate
                if (strlen(buf) > 0)
                    strncpy(normalizedStatus, buf, sizeof(normalizedStatus) - 1);
                else
                {
                    safe_send(client_fd, "❌ Invalid status. Please enter 'active' or 'inactive'.\n");
                    continue;
                }
            }

            char customerName[64] = {0};
            char customerStatus[16] = {0};
            int result = setCustomerStatus(custId, normalizedStatus, customerName, sizeof(customerName), customerStatus, sizeof(customerStatus));
            if (result)
            {
                char successMsg[256];
                snprintf(successMsg, sizeof(successMsg),
                         "✅ Customer status updated successfully.\n"
                         "   Customer ID: %d\n"
                         "   Customer Name: %s\n"
                         "   Status: %s\n",
                         custId, customerName, customerStatus);
                safe_send(client_fd, successMsg);
            }
            else
                safe_send(client_fd, "❌ Failed to update customer status (ID not found).\n");
        }
        // === Option 2: View Loans ===
        else if (!strcmp(buf, "2") || !strcmp(buf, "loans"))
        {
            showAllLoanApplications(client_fd);
        }
        // === Option 3: Assign Loan ===
        else if (!strcmp(buf, "3") || !strcmp(buf, "assignloan"))
        {
            char loanID[64];
            int empId;

            safe_send(client_fd, "Enter Loan ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            safe_copy(loanID, buf, sizeof(loanID));

            safe_send(client_fd, "Enter Employee ID to assign: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            empId = atoi(buf);
            if (empId <= 0)
            {
                safe_send(client_fd, "⚠️ Invalid Employee ID entered.\n");
                return;
            }
            int result = assignLoanToEmployee(loanID, empId);
            if (result == 1)
                safe_send(client_fd, "✅ Loan successfully assigned to employee.\n");
            else
                safe_send(client_fd, "❌ Failed to assign loan. Check Loan ID or Employee ID.\n");
        }

        // === Option 4: View Feedback ===
        else if (!strcmp(buf, "4") || !strcmp(buf, "feedback"))
        {
            viewFeedback(client_fd);
        }
        // === Option 5: Change Password ===
        else if (!strcmp(buf, "5") || !strcmp(buf, "changepass"))
        {
            safe_send(client_fd, "Enter old password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char oldPass[64];
            safe_copy(oldPass, buf, sizeof(oldPass));

            safe_send(client_fd, "Enter new password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char newPass[64];
            safe_copy(newPass, buf, sizeof(newPass));

            safe_send(client_fd,
                      changeManagerPassword(userId, oldPass, newPass)
                          ? "✅ Password changed successfully.\n"
                          : "❌ Incorrect old password.\n");
        }
        // === Option 6: Logout ===

        else if (strcmp(buf, "6") == 0 || strcasecmp(buf, "logout") == 0)
        {
            safe_send(client_fd, "👋 Logged out successfully. Returning to login page...\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        } // === Option 7: Exit ===
        else if (strcmp(buf, "7") == 0 || strcasecmp(buf, "exit") == 0)
        {
            safe_send(client_fd, "👋 Exiting program.\n 👋Good bye!\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            close(client_fd);
            exit(0);
        }
        // === Invalid Command ===
        else
        {
            safe_send(client_fd, "❌ Invalid command. Try again.\n");
        }
    }
}
