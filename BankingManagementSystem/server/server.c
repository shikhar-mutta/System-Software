#include "../include/common.h"
#include "../include/structures.h"
#include "../include/session.h"
#include "../include/employee.h"
#include "../include/customer.h"
#include "../include/manager.h"
#include "../include/admin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

// ---------------------------------------------------------
// Forward declarations (provided elsewhere)
int validateCustomerLogin(const char *id, const char *password);
int validateEmployeeLogin(const char *empId, const char *password, char *outRole, const char *userType);

// ---------------------------------------------------------
// Global tracking of clients for shutdown
// ---------------------------------------------------------
#define MAX_CLIENTS 128
static int client_fds[MAX_CLIENTS];
static int client_count = 0;
static int server_fd = -1;

// ---------------------------------------------------------
// 🧹 Graceful shutdown on Ctrl+C
// ---------------------------------------------------------
static void handle_sigint(int sig)
{
    (void)sig;
    fprintf(stderr, "\n🛑 Server shutting down... notifying all clients.\n");

    for (int i = 0; i < client_count; ++i)
    {
        if (client_fds[i] > 0)
        {
            const char *msg = "\n⚠️ Server shutting down. Please reconnect later.\n";
            send(client_fds[i], msg, strlen(msg), 0);
            shutdown(client_fds[i], SHUT_RDWR);
            close(client_fds[i]);
            client_fds[i] = -1;
        }
    }

    clearAllSessions();
    if (server_fd > 0)
        close(server_fd);

    // Kill all forked child processes
    kill(0, SIGTERM);

    fprintf(stderr, "✅ All client sessions terminated. Server closed.\n");
    exit(0);
}

// ---------------------------------------------------------
// 👶 Prevent zombie child processes
// ---------------------------------------------------------
static void reap_children(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

// ---------------------------------------------------------
// 🧾 Safe read line from socket (unchanged)
// ---------------------------------------------------------
static ssize_t safe_read_line(int fd, char *buf, size_t max)
{
    ssize_t r = read(fd, buf, max - 1);
    if (r <= 0)
    {
        if (r == 0)
            fprintf(stderr, "⚠️ Client (fd=%d) disconnected unexpectedly.\n", fd);
        return r;
    }

    buf[r] = '\0';
    char *p = strpbrk(buf, "\r\n");
    if (p)
        *p = '\0';
    return r;
}
static void safe_send(int fd, const char *msg)
{
    if (msg)
        send(fd, msg, strlen(msg), 0);
}

// ---------------------------------------------------------
// Helper: performLogin
// - Prompts the client for user type / id / password
// - Validates credentials (calls validateCustomerLogin / validateEmployeeLogin)
// - On success: adds session and fills userType, userId, displayName
// - Returns 1 on success, 0 on failure, -1 on client disconnect
// ---------------------------------------------------------
static int performLogin(int client_fd, char *outUserType, size_t userTypeLen,
                        char *outUserId, size_t userIdLen,
                        char *outDisplayName, size_t displayNameLen)
{
    char buffer[MAX];
    const char *menu =
        "\n=== Select User Type ===\n"
        "1. Customer\n"
        "2. Employee\n"
        "3. Manager\n"
        "4. Admin\n"
        "Enter choice (1-4): ";

    if (send(client_fd, menu, strlen(menu), 0) < 0)
        return -1;

    memset(buffer, 0, sizeof(buffer));
    if (safe_read_line(client_fd, buffer, sizeof(buffer)) <= 0)
        return -1;

    int choice = atoi(buffer);
    const char *userType = NULL;
    switch (choice)
    {
    case 1:
        userType = "customer";
        break;
    case 2:
        userType = "employee";
        break;
    case 3:
        userType = "manager";
        break;
    case 4:
        userType = "admin";
        break;
    default:
        send(client_fd, "❌ Invalid selection. Returning to menu...\n", strlen("❌ Invalid selection. Returning to menu...\n"), 0);
        return 0;
    }

    // Prompt for ID
    char idPrompt[64];
    if (strcmp(userType, "customer") == 0)
        snprintf(idPrompt, sizeof(idPrompt), "Enter Customer ID: ");
    else if (strcmp(userType, "employee") == 0)
        snprintf(idPrompt, sizeof(idPrompt), "Enter Employee ID: ");
    else if (strcmp(userType, "manager") == 0)
        snprintf(idPrompt, sizeof(idPrompt), "Enter Manager ID: ");
    else
        snprintf(idPrompt, sizeof(idPrompt), "Enter Admin ID: ");

    if (send(client_fd, idPrompt, strlen(idPrompt), 0) < 0)
        return -1;

    memset(buffer, 0, sizeof(buffer));
    if (safe_read_line(client_fd, buffer, sizeof(buffer)) <= 0)
        return -1;

    // copy userId
    strncpy(outUserId, buffer, userIdLen - 1);
    outUserId[userIdLen - 1] = '\0';

    // Prevent multiple logins
    if (isUserLoggedIn(outUserId))
    {
        send(client_fd, "⚠️ User already logged in!\n", strlen("⚠️ User already logged in!\n"), 0);
        return 0;
    }

    // Prompt for password
    if (send(client_fd, "Enter password: ", strlen("Enter password: "), 0) < 0)
        return -1;

    memset(buffer, 0, sizeof(buffer));
    if (safe_read_line(client_fd, buffer, sizeof(buffer)) <= 0)
        return -1;

    char password[64];
    strncpy(password, buffer, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';

    // Validate credentials
    int ok = 0;
    char roleFound[32] = {0};

    if (strcmp(userType, "customer") == 0)
    {
        ok = validateCustomerLogin(outUserId, password);
        if (ok > 0)
            strncpy(roleFound, "Customer", sizeof(roleFound) - 1);
        else if (ok == -1)
        {
            safe_send(client_fd, "⚠️ Your account is inactive. Contact the bank.\n");
            return 0;
        }
    }
    else
    {
        ok = validateEmployeeLogin(outUserId, password, roleFound, userType);
    }

    if (!ok)
    {
        send(client_fd, "❌ Invalid credentials.\n", strlen("❌ Invalid credentials.\n"), 0);
        return 0;
    }

    // Add session
    addSession(outUserId, roleFound[0] ? roleFound : userType);

    // Get username/display name from data files
    strncpy(outUserType, userType, userTypeLen - 1);
    outUserType[userTypeLen - 1] = '\0';

    // default display name
    strncpy(outDisplayName, "User", displayNameLen - 1);
    outDisplayName[displayNameLen - 1] = '\0';

    FILE *fp = NULL;
    int idnum = atoi(outUserId);

    if (strcmp(userType, "customer") == 0)
        fp = fopen("../data/customers.txt", "r");
    else
        fp = fopen("../data/employees.txt", "r");

    if (fp)
    {
        int cid;
        char uname[64], pass[64], extra[128];
        float bal;

        if (strcmp(userType, "customer") == 0)
        {
            while (fscanf(fp, "%d %63s %63s %f %127s", &cid, uname, pass, &bal, extra) == 5)
            {
                if (cid == idnum)
                {
                    strncpy(outDisplayName, uname, displayNameLen - 1);
                    outDisplayName[displayNameLen - 1] = '\0';
                    break;
                }
            }
        }
        else
        {
            while (fscanf(fp, "%d %63s %63s %127s", &cid, uname, pass, extra) == 4)
            {
                if (cid == idnum)
                {
                    strncpy(outDisplayName, uname, displayNameLen - 1);
                    outDisplayName[displayNameLen - 1] = '\0';
                    break;
                }
            }
        }

        fclose(fp);
    }

    // success
    return 1;
}

// ---------------------------------------------------------
// 🏦 Main Server Entry Point
// ---------------------------------------------------------
int main(void)
{
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    // char buffer[MAX];

    // --- Signal setup ---
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    signal(SIGCHLD, reap_children);
    clearAllSessions();

    // --- Create server socket ---
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 8) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("💻 Server started on port %d.\n", PORT);

    // ---------------------------------------------------------
    // 🧠 Main accept loop
    // ---------------------------------------------------------
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0)
        {
            if (errno == EINTR)
                continue;
            perror("accept");
            continue;
        }

        if (client_count < MAX_CLIENTS)
            client_fds[client_count++] = client_fd;

        printf("✅ New client connected (fd=%d)\n", client_fd);

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0)
        {
            // --- CHILD PROCESS ---
            close(server_fd);

            while (1)
            {
                char userType[32] = {0};
                char userId[64] = {0};
                char displayName[64] = {0};

                int loginResult = performLogin(client_fd, userType, sizeof(userType),
                                               userId, sizeof(userId),
                                               displayName, sizeof(displayName));

                if (loginResult == -1)
                {
                    // client disconnected while logging in
                    break;
                }
                else if (loginResult == 0)
                {
                    // failed login or user already logged in — go back to menu
                    continue;
                }

                // success: launch appropriate dashboard
                printf("👤 %s (%s) logged in successfully (fd=%d)\n",
                       displayName, userType, client_fd);

                if (strcmp(userType, "customer") == 0)
                    handleCustomerMenu(client_fd, atoi(userId), displayName);
                else if (strcmp(userType, "employee") == 0)
                    handleEmployeeMenu(client_fd, atoi(userId), displayName);
                else if (strcmp(userType, "manager") == 0)
                    handleManagerMenu(client_fd, atoi(userId), displayName);
                else if (strcmp(userType, "admin") == 0)
                    handleAdminMenu(client_fd, atoi(userId), displayName);

                // When dashboard returns, user has likely logged out (or session ended).
                // remove their session (safe to call even if already removed).
                removeSession(userId);

                // continue -> show login menu again
            }

            fprintf(stderr, "⚠️ Client (fd=%d) disconnected or exited.\n", client_fd);

            // cleanup socket and exit child
            shutdown(client_fd, SHUT_RDWR);
            close(client_fd);
            exit(0);
        }

        // --- PARENT process ---
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
