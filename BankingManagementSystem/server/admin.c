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
    if (!username || !password || !role || strlen(username) == 0 || strlen(password) == 0)
        return 0;

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

    int lastId;
    int id;
    char name[64], pass[64], empRole[64];

    // 🔍 Read all employees to find last used ID
    while (fscanf(fp, "%d %63s %63s %63s", &id, name, pass, empRole) == 4)
        lastId = id;

    int newId = lastId + 1;

    // ✏️ Append new employee entry
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

    FILE *fp = fopen(filename, "r+");
    if (!fp)
        return 0;

    int id;
    char uname[64], pass[64], role[32], status[32];
    float bal;
    long pos;
    int updated = 0;

    while ((pos = ftell(fp)), 1)
    {
        // Customer file
        if (strstr(filename, "customers"))
        {
            if (fscanf(fp, "%d %63s %63s %f %31s", &id, uname, pass, &bal, status) != 5)
                break;
            if (id == userId)
            {
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%-5d %-10s %-10s %-10.2f %-10s\n",
                        id, uname, newPass, bal, status);
                fflush(fp);
                updated = 1;
                break;
            }
        }
        // Employee file
        else
        {
            if (fscanf(fp, "%d %63s %63s %31s", &id, uname, pass, role) != 4)
                break;
            if (id == userId)
            {
                fseek(fp, pos, SEEK_SET);
                fprintf(fp, "%-5d %-10s %-10s %-10s\n",
                        id, uname, newPass, role);
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
// 🧠 Manage User Roles (Change by Employee ID, send via socket)
// =========================================================
int changeUserRole(int client_fd, int userId, const char *newRole)
{
    if (!newRole || strlen(newRole) == 0)
        return 0;

    FILE *fp = fopen(EMPLOYEE_FILE, "r+");
    if (!fp)
    {
        safe_send(client_fd, "❌ Failed to access employee records.\n");
        return 0;
    }

    int id, changed = 0;
    char uname[64], pass[64], role[32];
    long pos;
    char msg[256];

    while ((pos = ftell(fp)), fscanf(fp, "%d %63s %63s %31s", &id, uname, pass, role) == 4)
    {
        if (id == userId)
        {
            fseek(fp, pos, SEEK_SET); // Move to the beginning of this record
            fprintf(fp, "%-5d %-15s %-15s %-15s\n", id, uname, pass, newRole);
            fflush(fp);

            snprintf(msg, sizeof(msg),
                     "✅ Role updated for Employee ID %d: %s → %s\n",
                     userId, role, newRole);
            safe_send(client_fd, msg);

            changed = 1;
            break;
        }
    }

    if (!changed)
    {
        snprintf(msg, sizeof(msg),
                 "⚠️ Employee ID %d not found. No changes made.\n", userId);
        safe_send(client_fd, msg);
    }

    fclose(fp);
    return changed;
}

// =========================================================
// 🔐 Change Admin Password (by ID)
// =========================================================
int changeAdminPassword(int adminId, const char *oldPass, const char *newPass)
{
    if (!oldPass || !newPass || strlen(newPass) == 0)
        return 0;

    FILE *fp = fopen(EMPLOYEE_FILE, "r+");
    if (!fp)
        return 0;

    int id, changed = 0;
    char uname[64], pass[64], role[32];
    long pos;

    while ((pos = ftell(fp)), fscanf(fp, "%d %63s %63s %31s", &id, uname, pass, role) == 4)
    {
        if (id == adminId && strcmp(pass, oldPass) == 0 && strcmp(role, "admin") == 0)
        {
            fseek(fp, pos, SEEK_SET);
            fprintf(fp, "%-5d %-15s %-15s %-10s\n", id, uname, newPass, role);
            fflush(fp);
            changed = 1;
            break;
        }
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
            removeSession(username);
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

            safe_send(client_fd,
                      addEmployee(uname, pass, strlen(buf) > 0 ? buf : "employee")
                          ? "✅ Employee added successfully.\n"
                          : "❌ Failed to add employee.\n");
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

            safe_send(client_fd,
                      changeUserRole(client_fd, empId, buf)
                          ? "✅ Role updated successfully.\n"
                          : "❌ Failed to change role.\n");
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
            removeSession(username);
            return;
        }

        // 7 Exit
        else if (strcmp(buf, "7") == 0 || strcasecmp(buf, "exit") == 0)
        {
            safe_send(client_fd, "👋 Exiting program.\n 👋Good bye!\n");
            removeSession(username);
            close(client_fd);
            exit(0);
        }

        else
            safe_send(client_fd, "❌ Invalid command. Try again.\n");
    }
}
