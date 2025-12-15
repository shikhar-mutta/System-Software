#include "../include/common.h"
#include <string.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>

#define PROMPT_BUF 4096
#define SOCKET_TIMEOUT_SEC 15 // allow long menus, e.g., manager or employee

static volatile sig_atomic_t keep_running = 1;
static int global_sock = -1;
static char userId[64] = ""; // Store user ID globally

// ----------------------------------------------------------------------
// 🧹 Graceful exit on Ctrl+C
// ----------------------------------------------------------------------
void handle_sigint(int sig)
{
    (void)sig;
    keep_running = 0;
    const char msg[] = "\n👋 Exiting program (Ctrl+C)...\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    if (global_sock != -1)
    {
        // Close socket immediately to trigger server disconnect detection
        // This ensures the server removes the session from sessions.txt
        shutdown(global_sock, SHUT_RDWR);
        close(global_sock);
        global_sock = -1;
    }

    exit(0);
}

// ----------------------------------------------------------------------
// ⚙️ Socket setup and optimization
// ----------------------------------------------------------------------
void setupSocketOptions(int sock)
{
    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    struct timeval tv = {SOCKET_TIMEOUT_SEC, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

// ----------------------------------------------------------------------
// 📜 Read until an interactive prompt or disconnect
// ----------------------------------------------------------------------
int read_until_prompt(int sock, char *out_buf, int out_sz)
{
    int total = 0;
    while (keep_running)
    {
        char tmp[1024];
        ssize_t r = read(sock, tmp, sizeof(tmp) - 1);

        if (r == 0)
        {
            printf("\n❌ Server closed the connection.\n");
            printf("👋 Exiting client automatically...\n");

            if (global_sock != -1)
            {
                shutdown(global_sock, SHUT_RDWR);
                close(global_sock);
                global_sock = -1;
            }

            exit(0); // 🔥 Exit immediately (no return to main)
        }

        if (r < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            perror("Read error");

            if (global_sock != -1)
            {
                shutdown(global_sock, SHUT_RDWR);
                close(global_sock);
                global_sock = -1;
            }

            exit(0);
        }

        tmp[r] = '\0';
        fwrite(tmp, 1, r, stdout);
        fflush(stdout);

        // Copy into accumulation buffer
        if (out_buf && out_sz > 0)
        {
            int to_copy = (r < out_sz - total - 1) ? r : (out_sz - total - 1);
            if (to_copy > 0)
            {
                memcpy(out_buf + total, tmp, to_copy);
                total += to_copy;
                out_buf[total] = '\0';
            }
        }

        // --- Detect server shutdown / disconnect messages ---
        if (strstr(tmp, "Server shutting down") ||
            strstr(tmp, "Disconnecting") ||
            strstr(tmp, "Please reconnect later") ||
            strstr(tmp, "Connection closed"))
        {
            printf("\n⚠️ Server is shutting down. Closing client gracefully...\n");

            if (global_sock != -1)
            {
                shutdown(global_sock, SHUT_RDWR);
                close(global_sock);
                global_sock = -1;
            }

            exit(0); // 🔥 Exit immediately when server closes
        }

        // --- Detect interactive prompt ---
        if (strstr(tmp, "Enter ") ||
            strstr(tmp, "choice") ||
            strstr(tmp, "Option") ||
            strstr(tmp, "password") ||
            strstr(tmp, "Confirm") ||
            strstr(tmp, "Logout") ||
            strstr(tmp, "Exit") ||
            strstr(tmp, "Loan ID") ||
            strstr(tmp, "customer name") ||
            strstr(tmp, "decision"))
        {
            usleep(50000);                   // allow prompt to fully print
            tcflush(STDIN_FILENO, TCIFLUSH); // clear input buffer
            return 1;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------
// 📤 Send input safely to server
// ----------------------------------------------------------------------
int send_input(int sock, const char *input)
{
    if (!input)
        return 0;

    size_t len = strlen(input);
    if (len == 0)
        return 1;

    char buffer[MAX];
    strncpy(buffer, input, sizeof(buffer) - 2);
    buffer[sizeof(buffer) - 2] = '\0';
    if (buffer[strlen(buffer) - 1] != '\n')
        strcat(buffer, "\n");

    ssize_t sent = send(sock, buffer, strlen(buffer), 0);
    if (sent <= 0)
    {
        perror("Send failed");
        return 0;
    }
    return 1;
}

// ----------------------------------------------------------------------
// 🧠 Helper: trim input (remove newline)
// ----------------------------------------------------------------------
void trim(char *s)
{
    if (!s)
        return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
        s[--len] = '\0';
}

// ----------------------------------------------------------------------
// 🚀 Main Client Logic
// ----------------------------------------------------------------------
int main()
{
    signal(SIGINT, handle_sigint);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    global_sock = sock;
    setupSocketOptions(sock);

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(sock);
        return 1;
    }

    printf("🚀 Connecting to %s:%d ...\n", SERVER_IP, PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("✅ Connected to server %s:%d\n\n", SERVER_IP, PORT);

    char buffer[PROMPT_BUF];

    while (keep_running)
    {
        memset(buffer, 0, sizeof(buffer));

        // Wait for prompt or shutdown message
        int gotPrompt = read_until_prompt(sock, buffer, sizeof(buffer));
        if (!gotPrompt)
        {
            printf("\n🔌 Connection closed or lost.\n");
            break;
        }

        // Detect logout / end messages
        if (
            strstr(buffer, "Exiting program") ||
            strstr(buffer, "Disconnecting") ||
            strstr(buffer, "Goodbye"))
        {
            printf("\n👋 Session ended by server.\n");
            break;
        }

        // --- Get user input from console ---
        char input[MAX];
        memset(input, 0, sizeof(input));

        tcflush(STDIN_FILENO, TCIFLUSH);
        if (!fgets(input, sizeof(input), stdin))
        {
            printf("\n🔌 Input stream closed by user.\n");
            break;
        }

        trim(input);

        // ✅ Capture User ID during login
        if (strstr(buffer, "Enter Customer ID") ||
            strstr(buffer, "Enter Employee ID") ||
            strstr(buffer, "Enter Manager ID") ||
            strstr(buffer, "Enter Admin ID"))
        {
            strncpy(userId, input, sizeof(userId) - 1);
            userId[sizeof(userId) - 1] = '\0';
        }

        if (!send_input(sock, input))
        {
            printf("❌ Failed to send input.\n");
            break;
        }
    }

    if (global_sock != -1)
    {
        shutdown(global_sock, SHUT_RDWR);
        close(global_sock);
        global_sock = -1;
    }

    printf("🔌 Disconnected from server. Good bye!\n");
    return 0;
}
