#include "../include/admin.h"
#include "../include/session.h"
#include "../include/structures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// =========================================================
// 📂 File Paths
// =========================================================
#define CUSTOMER_FILE "../data/customers.txt"
#define EMPLOYEE_FILE "../data/employees.txt"

// =========================================================
// 🔧 Utility Helpers
// =========================================================

static void trim(char *s)
{
    if (!s)
        return;
    while (*s && (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r'))
        memmove(s, s + 1, strlen(s));
    while (strlen(s) > 0)
    {
        char last = s[strlen(s) - 1];
        if (last == ' ' || last == '\t' || last == '\n' || last == '\r')
            s[strlen(s) - 1] = '\0';
        else
            break;
    }
}

static void safe_send(int fd, const char *msg)
{
    if (msg)
        send(fd, msg, strlen(msg), 0);
}

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

static inline void sync_delay() { usleep(20000); }

// =========================================================
// 🧩 Add New Bank Employee (auto-increment 4-digit ID)
// =========================================================
int addEmployee(const char *username, const char *password, const char *role)
{
    if (!username || !password || !role || 
        strlen(username) == 0 || strlen(password) == 0 || strlen(role) == 0)
        return 0;
    
    // Validate role is one of the allowed values
    if (strcmp(role, "employee") != 0 && 
        strcmp(role, "manager") != 0 && 
        strcmp(role, "admin") != 0)
    {
        return 0; // Invalid role
    }

    FILE *fp = fopen(EMPLOYEE_FILE, "a+");
    if (!fp)
        return 0;

    int fd = fileno(fp);

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("Lock failed");
        fclose(fp);
        return 0;
    }

    rewind(fp); // start reading from the beginning

    int lastId = 2000; // Start from 2000 for employee IDs if file is empty
    int id;
    char name[64], pass[64], empRole[64];

    // 🔍 Read all employees to find last used ID (skip malformed lines)
    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (sscanf(line, "%d %63s %63s %63s", &id, name, pass, empRole) == 4)
        {
            if (id > lastId)
                lastId = id;
        }
    }

    int newId = lastId + 1;

    // ✏️ Append new employee entry with proper formatting
    fprintf(fp, "%d %s %s %s\n", newId, username, password, role);
    fflush(fp);
    fsync(fd); // ensure data is physically written

    // 🔓 Unlock file
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(fp);

    printf("✅ New employee added with ID: %d (%s - %s)\n", newId, username, role);

    return newId; // return the newly assigned ID
}

// =========================================================
// 🔧 Modify Customer or Employee Details (by user ID)
// =========================================================
int modifyUserDetailsById(const char *filename, int userId, const char *newPass)
{
    if (!filename || !newPass)
        return 0;

    FILE *fp = fopen(filename, "r");
    if (!fp)
        return 0;

    int updated = 0;
    int isCustomerFile = (strstr(filename, "customers") != NULL);

    if (isCustomerFile)
    {
        // Customer file format: ID username password balance status
        typedef struct
        {
            int id;
            char uname[64];
            char pass[64];
            float bal;
            char status[32];
        } Cust;

        Cust customers[500];
        int count = 0;

        // Read all customers
        while (count < 500 && fscanf(fp, "%d %63s %63s %f %31s",
                                     &customers[count].id,
                                     customers[count].uname,
                                     customers[count].pass,
                                     &customers[count].bal,
                                     customers[count].status) == 5)
        {
            if (customers[count].id == userId)
            {
                strncpy(customers[count].pass, newPass, sizeof(customers[count].pass) - 1);
                customers[count].pass[sizeof(customers[count].pass) - 1] = '\0';
                updated = 1;
            }
            count++;
        }
        fclose(fp);

        if (!updated)
            return 0;

        // Write all customers back
        fp = fopen(filename, "w");
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
    }
    else
    {
        // Employee file format: ID username password role
        typedef struct
        {
            int id;
            char uname[64];
            char pass[64];
            char role[32];
        } Emp;

        Emp employees[500];
        int count = 0;

        // Read all employees
        while (count < 500 && fscanf(fp, "%d %63s %63s %31s",
                                     &employees[count].id,
                                     employees[count].uname,
                                     employees[count].pass,
                                     employees[count].role) == 4)
        {
            if (employees[count].id == userId)
            {
                strncpy(employees[count].pass, newPass, sizeof(employees[count].pass) - 1);
                employees[count].pass[sizeof(employees[count].pass) - 1] = '\0';
                updated = 1;
            }
            count++;
        }
        fclose(fp);

        if (!updated)
            return 0;

        // Write all employees back
        fp = fopen(filename, "w");
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
    }

    return updated;
}

// =========================================================
// 🧠 Manage User Roles (Change by Employee ID, send via socket)
// =========================================================
int changeUserRole(int client_fd, int userId, const char *newRole)
{
    if (!newRole || strlen(newRole) == 0)
        return 0;

    FILE *fp = fopen(EMPLOYEE_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "❌ Failed to access employee records.\n");
        return 0;
    }

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
    char oldRole[32] = {0};
    char msg[256];

    // Read all employees
    while (count < 500 && fscanf(fp, "%d %63s %63s %31s",
                                 &employees[count].id,
                                 employees[count].uname,
                                 employees[count].pass,
                                 employees[count].role) == 4)
    {
        if (employees[count].id == userId)
        {
            strncpy(oldRole, employees[count].role, sizeof(oldRole) - 1);
            oldRole[sizeof(oldRole) - 1] = '\0';
            // Update role
            strncpy(employees[count].role, newRole, sizeof(employees[count].role) - 1);
            employees[count].role[sizeof(employees[count].role) - 1] = '\0';
            changed = 1;
        }
        count++;
    }
    fclose(fp);

    if (!changed)
    {
        snprintf(msg, sizeof(msg),
                 "⚠️ Employee ID %d not found. No changes made.\n", userId);
        safe_send(client_fd, msg);
        return 0;
    }

    // Write all employees back with proper formatting (same format as addEmployee)
    fp = fopen(EMPLOYEE_FILE, "w");
    if (!fp)
    {
        safe_send(client_fd, "❌ Failed to write employee records.\n");
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "%d %s %s %s\n",
                employees[i].id,
                employees[i].uname,
                employees[i].pass,
                employees[i].role);
    }
    fclose(fp);

    snprintf(msg, sizeof(msg),
             "✅ Role updated for Employee ID %d: %s → %s\n",
             userId, oldRole, newRole);
    safe_send(client_fd, msg);

    return changed;
}

// =========================================================
// 🔐 Change Admin Password (by ID)
// =========================================================
int changeAdminPassword(int adminId, const char *oldPass, const char *newPass)
{
    if (!oldPass || !newPass || strlen(newPass) == 0)
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
        if (employees[count].id == adminId && 
            strcmp(employees[count].pass, oldPass) == 0 && 
            strcmp(employees[count].role, "admin") == 0)
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
// 🧑‍💼 Admin Dashboard Menu
// =========================================================
void handleAdminMenu(int client_fd, const int userId, const char *username)
{
    char buf[MAX], msg[512];

    while (1)
    {
        snprintf(msg, sizeof(msg),
                 "\n=== 🛡️ Welcome, %s (Administrator) ===\n"
                 "--------------------------------------------\n"
                 "1. addemp      - Add New Employee\n"
                 "2. modifycust  - Modify Customer Password\n"
                 "3. modifyemp   - Modify Employee Password\n"
                 "4. changerole  - Change Employee Role\n"
                 "5. changepass  - Change Admin Password\n"
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

        // 1 Add Employee
        if (!strcmp(buf, "1") || !strcmp(buf, "addemp"))
        {
            safe_send(client_fd, "Enter employee username: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char uname[64];
            strcpy(uname, buf);

            safe_send(client_fd, "Enter password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            char pass[64];
            strcpy(pass, buf);

            safe_send(client_fd, "Enter role (employee/manager): ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            // Normalize role input: accept "employee"/"e" or "manager"/"m" (case-insensitive)
            char normalizedRole[32] = {0};
            if (strlen(buf) == 0)
            {
                // Default to employee if empty
                strcpy(normalizedRole, "employee");
            }
            else if (strcasecmp(buf, "e") == 0 || strcasecmp(buf, "employee") == 0)
            {
                strcpy(normalizedRole, "employee");
            }
            else if (strcasecmp(buf, "m") == 0 || strcasecmp(buf, "manager") == 0)
            {
                strcpy(normalizedRole, "manager");
            }
            else
            {
                // Invalid input - reject and ask again
                safe_send(client_fd, "❌ Invalid role. Please enter 'employee' or 'manager' (or 'e'/'m').\n");
                continue;
            }

            // Validate username and password are not empty
            if (strlen(uname) == 0)
            {
                safe_send(client_fd, "❌ Username cannot be empty.\n");
                continue;
            }
            if (strlen(pass) == 0)
            {
                safe_send(client_fd, "❌ Password cannot be empty.\n");
                continue;
            }

            int newEmployeeId = addEmployee(uname, pass, normalizedRole);
            if (newEmployeeId > 0)
            {
                char successMsg[256];
                snprintf(successMsg, sizeof(successMsg),
                         "✅ Employee added successfully.\n"
                         "   Employee ID: %d\n"
                         "   Username: %s\n"
                         "   Role: %s\n",
                         newEmployeeId, uname, normalizedRole);
                safe_send(client_fd, successMsg);
            }
            else
            {
                safe_send(client_fd, "❌ Failed to add employee.\n");
            }
        }

        // 2 Modify Customer Password (by ID)
        else if (!strcmp(buf, "2") || !strcmp(buf, "modifycust"))
        {
            safe_send(client_fd, "Enter Customer ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            int custId = atoi(buf);

            if (custId <= 0)
            {
                safe_send(client_fd, "❌ Invalid Customer ID.\n");
                return;
            }

            safe_send(client_fd, "Enter new password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int updated = modifyUserDetailsById(CUSTOMER_FILE, custId, buf);

            safe_send(client_fd,
                      updated
                          ? "✅ Customer password updated successfully.\n"
                          : "❌ Customer ID not found.\n");
        }

        // 3 Modify Employee Password (by ID)
        else if (!strcmp(buf, "3") || !strcmp(buf, "modifyemp"))
        {
            safe_send(client_fd, "Enter Employee ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            int empId = atoi(buf);

            if (empId <= 0)
            {
                safe_send(client_fd, "❌ Invalid Employee ID.\n");
                return;
            }

            safe_send(client_fd, "Enter new password: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            int updated = modifyUserDetailsById(EMPLOYEE_FILE, empId, buf);

            safe_send(client_fd,
                      updated
                          ? "✅ Employee password updated successfully.\n"
                          : "❌ Employee ID not found.\n");
        }

        // 4 Change Role
        else if (!strcmp(buf, "4") || !strcmp(buf, "changerole"))
        {
            safe_send(client_fd, "Enter Employee ID: ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);
            int empId = atoi(buf);

            if (empId <= 0)
            {
                safe_send(client_fd, "❌ Invalid Employee ID.\n");
                return;
            }

            safe_send(client_fd, "Enter new role (employee/manager/admin): ");
            read_line(client_fd, buf, sizeof(buf));
            trim(buf);

            // Normalize role input: accept "employee"/"e", "manager"/"m", or "admin"/"a" (case-insensitive)
            char normalizedRole[32] = {0};
            if (strlen(buf) == 0)
            {
                safe_send(client_fd, "❌ Invalid role. Please enter 'employee', 'manager', or 'admin'.\n");
                continue;
            }
            else if (strcasecmp(buf, "e") == 0 || strcasecmp(buf, "employee") == 0)
            {
                strcpy(normalizedRole, "employee");
            }
            else if (strcasecmp(buf, "m") == 0 || strcasecmp(buf, "manager") == 0)
            {
                strcpy(normalizedRole, "manager");
            }
            else if (strcasecmp(buf, "a") == 0 || strcasecmp(buf, "admin") == 0)
            {
                strcpy(normalizedRole, "admin");
            }
            else
            {
                // If input doesn't match, use as-is but show warning
                strncpy(normalizedRole, buf, sizeof(normalizedRole) - 1);
                normalizedRole[sizeof(normalizedRole) - 1] = '\0';
            }

            changeUserRole(client_fd, empId, normalizedRole);
        }

        // 5 Change Admin Password
        else if (!strcmp(buf, "5") || !strcmp(buf, "changepass"))
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

            safe_send(client_fd,
                      changeAdminPassword(userId, oldPass, newPass)
                          ? "✅ Password changed successfully.\n"
                          : "❌ Incorrect old password.\n");
        }

        // 6 Logout
        else if (strcmp(buf, "6") == 0 || strcasecmp(buf, "logout") == 0)
        {
            safe_send(client_fd, "👋 Logged out successfully. Returning to login page...\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        }

        // 7 Exit
        else if (strcmp(buf, "7") == 0 || strcasecmp(buf, "exit") == 0)
        {
            safe_send(client_fd, "👋 Exiting program.\n 👋Good bye!\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            close(client_fd);
            exit(0);
        }

        else
            safe_send(client_fd, "❌ Invalid command. Try again.\n");
    }
}
