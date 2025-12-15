#include "../include/common.h"
#include "../include/structures.h"
#include "../include/session.h"

#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#define CUSTOMER_FILE "../data/customers.txt"
#define TRANSACTION_FILE "../data/transactions.txt"
#define LOAN_FILE "../data/loans.txt"
#define FEEDBACK_FILE "../data/feedback.txt"

// ---------------------
// Utility: Trim string safely
void trimInput(char *str)
{
    if (!str)
        return;
    str[strcspn(str, "\r\n")] = '\0';
    while (strlen(str) > 0 && isspace((unsigned char)str[strlen(str) - 1]))
        str[strlen(str) - 1] = '\0';
    size_t start = 0;
    while (isspace((unsigned char)str[start]))
        start++;
    if (start > 0)
        memmove(str, str + start, strlen(str + start) + 1);
}

// Safe send wrapper - returns 0 on failure (socket closed), 1 on success
static int safe_send_check(int fd, const char *msg)
{
    if (!msg)
        return 1;
    ssize_t sent = send(fd, msg, strlen(msg), 0);
    if (sent < 0)
    {
        // Socket closed or error
        if (errno == EPIPE || errno == ECONNRESET)
            return 0; // Socket closed
    }
    return 1; // Success
}

// Safe send wrapper (for backward compatibility - ignores return value)
static void safe_send(int fd, const char *msg)
{
    if (msg && fd >= 0)
    {
        ssize_t sent = send(fd, msg, strlen(msg), 0);
        if (sent < 0 && errno != EPIPE && errno != ECONNRESET)
        {
            // Log error but don't crash - connection might be closed
            // errno will be set appropriately
        }
    }
}
ssize_t safe_read_line(int fd, char *buf, size_t max)
{
    if (!buf || max == 0 || fd < 0)
        return -1;
        
    ssize_t r = read(fd, buf, max - 1);
    if (r <= 0)
    {
        if (r == 0)
            return 0; // Disconnected
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            return -1; // Retry needed
        return -1; // Error
    }

    buf[r] = '\0';

    // Trim CRLF
    char *p = strpbrk(buf, "\r\n");
    if (p)
        *p = '\0';

    return r;
}

int lockFile(int fd, short lockType)
{
    if (fd < 0)
        return -1;
        
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = lockType; // F_WRLCK, F_RDLCK, or F_UNLCK
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;                    // Lock entire file
    int result = fcntl(fd, F_SETLKW, &lock); // Wait until lock is acquired
    if (result == -1 && errno != EINTR)
    {
        // Lock failed
        return -1;
    }
    return result;
}

// ---------------------
// Helper: Get Balance
// ---------------------
float getBalance(const int userId)
{
    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp)
        return -1;

    int id;
    char uname[50], pass[50], status[10];
    float bal;

    while (fscanf(fp, "%d %49s %49s %f %9s",
                  &id, uname, pass, &bal, status) == 5)
    {
        if (id == userId)
        {
            fclose(fp);
            return bal;
        }
    }
    fclose(fp);
    return -1;
}

// ---------------------
// Helper: Update Balance (in-place rewrite)
// ---------------------
int updateBalance(const int userId, float newBalance)
{
    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp)
        return 0;

    typedef struct
    {
        int id;
        char uname[50];
        char pass[50];
        float bal;
        char status[10];
    } Cust;

    Cust customers[500];
    int count = 0;

    int changed = 0;
    while (count < 500 && fscanf(fp, "%d %49s %49s %f %9s",
                                 &customers[count].id,
                                 customers[count].uname,
                                 customers[count].pass,
                                 &customers[count].bal,
                                 customers[count].status) == 5)
    {
        if (customers[count].id == userId)
        {
            customers[count].bal = newBalance;
            changed = 1;
        }
        count++;
    }
    
    if (ferror(fp))
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    if (!changed)
        return 0;

    fp = fopen(CUSTOMER_FILE, "w");
    if (!fp)
        return 0;
    
    for (int i = 0; i < count; i++)
    {
        if (fprintf(fp, "%d %s %s %.2f %s\n",
                    customers[i].id,
                    customers[i].uname,
                    customers[i].pass,
                    customers[i].bal,
                    customers[i].status) < 0)
        {
            fclose(fp);
            return 0;
        }
    }
    
    if (fflush(fp) != 0 || ferror(fp))
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

// ---------------------
// Helper: Append Transaction
// ---------------------
int appendTransaction(const char *username, const char *type, float amount, float balance_after, int userId)
{
    FILE *fp = fopen(TRANSACTION_FILE, "a");
    if (!fp)
        return 0;

    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
    {
        perror("gettimeofday failed");
        fclose(fp);
        return 0;
    }
    
    char timestr[64];
    struct tm *tm_info = localtime(&tv.tv_sec);
    if (!tm_info)
    {
        fclose(fp);
        return 0;
    }
    
    if (strftime(timestr, sizeof(timestr), "%Y-%m-%d_%H:%M:%S", tm_info) == 0)
    {
        fclose(fp);
        return 0;
    }
    
    int result = 0;
    if (userId)
    {
        result = fprintf(fp, "TXN_%ld %s %s %.2f %s %.2f %d\n",
                        tv.tv_sec, username, type, amount, timestr, balance_after, userId);
    }
    else
    {
        result = fprintf(fp, "TXN_%ld %s %s %.2f %s %.2f\n",
                        tv.tv_sec, username, type, amount, timestr, balance_after);
    }
    
    if (result < 0 || fflush(fp) != 0 || ferror(fp))
    {
        fclose(fp);
        return 0;
    }
    
    fclose(fp);
    return 1;
}

// ---------------------
// Helper: Transfer Funds (Atomic + Advisory Locking)
// ---------------------
int transferFunds(const char *fromUser, const int toUserId,
                  float amount, float *fromNewBal, float *toNewBal, char *toUser)
{
    if (amount <= 0)
        return 0;

    int fd = open(CUSTOMER_FILE, O_RDWR);
    if (fd < 0)
    {
        perror("open failed");
        return 0;
    }

    // ---- Lock the file exclusively ----
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("Lock failed");
        close(fd);
        return 0;
    }

    FILE *fp = fdopen(fd, "r+");
    if (!fp)
    {
        perror("fdopen failed");
        close(fd);
        return 0;
    }

    typedef struct
    {
        int id;
        char uname[50];
        char pass[50];
        float bal;
        char status[10];
    } Cust;

    Cust customers[500];
    int count = 0;

    rewind(fp);
    while (count < 500 && fscanf(fp, "%d %49s %49s %f %9s",
                                 &customers[count].id,
                                 customers[count].uname,
                                 customers[count].pass,
                                 &customers[count].bal,
                                 customers[count].status) == 5)
        count++;

    int fromIdx = -1, toIdx = -1;
    for (int i = 0; i < count; i++)
    {
        // Match sender by username
        if (strcasecmp(customers[i].uname, fromUser) == 0)
        {
            fromIdx = i;
        }

        // Match recipient by ID
        if (customers[i].id == toUserId)
        {
            toIdx = i;
            strcpy(toUser, customers[toIdx].uname);
        }
    }

    if (fromIdx == -1 || toIdx == -1)
    {
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(fp);
        return 0;
    }

    if (customers[fromIdx].bal < amount)
    {
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(fp);
        return 0;
    }

    // ---- Perform the transfer ----
    customers[fromIdx].bal -= amount;
    customers[toIdx].bal += amount;
    *fromNewBal = customers[fromIdx].bal;
    *toNewBal = customers[toIdx].bal;

    // ---- Rewrite using same fd (avoid freopen) ----
    if (ftruncate(fd, 0) != 0)
    {
        perror("ftruncate failed");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(fp);
        return 0;
    }
    
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        perror("lseek failed");
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(fp);
        return 0;
    }

    for (int i = 0; i < count; i++)
    {
        if (dprintf(fd, "%d %s %s %.2f %s\n",
                    customers[i].id,
                    customers[i].uname,
                    customers[i].pass,
                    customers[i].bal,
                    customers[i].status) < 0)
        {
            perror("dprintf failed");
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            fclose(fp);
            return 0;
        }
    }

    if (fsync(fd) != 0)
    {
        perror("fsync failed");
        // Continue anyway - data might be written
    }

    // ---- Unlock file ----
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(fp); // closes fd too

    return 1;
}

// ---------------------
// Helper: Apply Loan
// ---------------------
int applyForLoan(const char *username, float amount, const char *reason, int userId)
{
    if (!username || !reason || amount <= 0)
        return 0;

    FILE *fp = fopen(LOAN_FILE, "a");
    if (!fp)
    {
        perror("Failed to open loan file");
        return 0;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    // Construct loan ID (e.g., "L1730489123")
    char loanId[32];
    snprintf(loanId, sizeof(loanId), "L%ld", tv.tv_sec);

    // Default status and employee IDs
    const char *status = "pending";
    int assignedEmpId = 0;
    int approvedEmpId = 0;

    // Write formatted entry to file
    fprintf(fp, "%s %04d %s %.2f %s %s %04d %04d\n",
            loanId,        // Loan ID
            userId,        // Customer ID
            username,      // Customer Name
            amount,        // Loan Amount
            status,        // Loan Status
            reason,        // Reason for loan
            assignedEmpId, // Assigned Employee ID
            approvedEmpId  // Approved Employee ID
    );

    fclose(fp);
    return 1;
}

// ---------------------
// Helper: Change Password
// ---------------------
int changePassword(const char *username, const char *oldPass, const char *newPass)
{
    FILE *fp = fopen(CUSTOMER_FILE, "r");
    if (!fp)
        return 0;

    typedef struct
    {
        int id;
        char uname[50];
        char pass[50];
        float bal;
        char status[10];
    } Cust;

    Cust customers[500];
    int count = 0, changed = 0;
    while (count < 500 && fscanf(fp, "%d %49s %49s %f %9s",
                                 &customers[count].id,
                                 customers[count].uname,
                                 customers[count].pass,
                                 &customers[count].bal,
                                 customers[count].status) == 5)
    {
        if (strcasecmp(customers[count].uname, username) == 0 &&
            strcmp(customers[count].pass, oldPass) == 0)
        {
            strcpy(customers[count].pass, newPass);
            changed = 1;
        }
        count++;
    }
    fclose(fp);

    if (!changed)
        return 0;

    fp = fopen(CUSTOMER_FILE, "w");
    if (!fp)
        return 0;
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d %s %s %.2f %s\n",
                customers[i].id,
                customers[i].uname,
                customers[i].pass,
                customers[i].bal,
                customers[i].status);
    fclose(fp);
    return 1;
}

// ---------------------
// Helper: Feedback
// ---------------------
int submitFeedback(const char *username, const char *message, int userId)
{
    FILE *fp = fopen(FEEDBACK_FILE, "a");
    if (!fp)
        return 0;

    // Get current timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    char timestr[64];
    struct tm *tm_info = localtime(&tv.tv_sec);
    strftime(timestr, sizeof(timestr), "%Y-%m-%d_%H:%M:%S", tm_info);

    // Write formatted feedback: [timestamp] username(userId): message
    fprintf(fp, "[%s] %s(%d): %s\n", timestr, username, userId, message);

    fclose(fp);
    return 1;
}

// ---------------------
// Helper: Show Transaction History (optimized for new format)
// ---------------------
void showTransactionHistory(int client_fd, int userId)
{
    FILE *fp = fopen(TRANSACTION_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No transaction history found.\n");
        return;
    }

    char output[8192]; // buffer for combined output
    memset(output, 0, sizeof(output));

    char line[512];
    int found = 0;

    strcat(output, "\n📜 Transaction History:\n");
    strcat(output, "====================================================\n");

    while (fgets(line, sizeof(line), fp))
    {
        char txId[64];
        char username[64];
        char type[32];
        float amount, totalBal;
        char datetime[64];
        int custId;

        // Match new structure:
        // TXN_ID, username, type, amount, datetime, total_balance, cust_ID
        int parsed = sscanf(line, "%63s %63s %31s %f %63s %f %d",
                            txId, username, type, &amount, datetime, &totalBal, &custId);

        if (parsed == 7 && custId == userId)
        {
            found = 1;

            char entry[512];
            snprintf(entry, sizeof(entry),
                     "🧾 Transaction ID: %s\n"
                     "👤 User: %s\n"
                     "📂 Type: %s\n"
                     "💰 Amount: ₹%.2f\n"
                     "📅 Date/Time: %s\n"
                     "🏦 Balance After: ₹%.2f\n"
                     "----------------------------------------------------\n",
                     txId, username, type, amount, datetime, totalBal);

            // Append entry safely
            if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                strcat(output, entry);
            else
                break; // prevent overflow if many transactions
        }
    }

    fclose(fp);

    if (!found)
        strcat(output, "⚠️ You have no recorded transactions yet.\n");

    strcat(output, "====================================================\n\n");

    // Send all at once for smoother client display
    safe_send(client_fd, output);
}

// ---------------------
// Helper: Show Customer Loans (optimized)
// ---------------------
void showCustomerLoans(int client_fd, const char *username, int userId)
{
    FILE *fp = fopen(LOAN_FILE, "r");
    if (!fp)
    {
        safe_send(client_fd, "⚠️ No loan applications found.\n");
        return;
    }

    char output[8192]; // large buffer for all loans
    memset(output, 0, sizeof(output));

    char line[512];
    int found = 0;

    strcat(output, "\n📄 Your Loan Applications:\n");
    strcat(output, "====================================================\n");

    while (fgets(line, sizeof(line), fp))
    {
        char loanId[30];
        int custId;
        char custName[50];
        float amount;
        char status[20];
        char reason[100];
        int assignedEmpId;
        int approvedEmpId;

        // Parse full loan structure
        int parsed = sscanf(line, "%s %d %s %f %s %s %d %d",
                            loanId,
                            &custId,
                            custName,
                            &amount,
                            status,
                            reason,
                            &assignedEmpId,
                            &approvedEmpId);

        if (parsed == 8 && custId == userId)
        {
            found = 1;

            char entry[512];
            snprintf(entry, sizeof(entry),
                     "🏦 Loan ID: %s\n"
                     "💰 Amount: ₹%.2f\n"
                     "📊 Status: %s\n"
                     "📝 Reason: %s\n"
                     "👷 Assigned Employee ID: %04d\n"
                     "✅ Approved Employee ID: %04d\n"
                     "----------------------------------------------------\n",
                     loanId, amount, status, reason, assignedEmpId, approvedEmpId);

            // Safely append the entry to the main buffer
            if (strlen(output) + strlen(entry) < sizeof(output) - 1)
                strcat(output, entry);
            else
                break; // prevent buffer overflow
        }
    }

    fclose(fp);

    if (!found)
        strcat(output, "⚠️ You have not applied for any loans yet.\n");

    strcat(output, "====================================================\n\n");

    // Send everything in one go
    safe_send(client_fd, output);
}

// ---------------------
// Main Customer Menu
// ---------------------
void handleCustomerMenu(int client_fd, const int userId, const char *username)
{
    char buffer[MAX], msg[512];

    while (1)
    {
        snprintf(msg, sizeof(msg),
                 "\n=== 👤 Welcome, %s (Customer) ===\n"
                 "--------------------------------------------\n"
                 " 1. view        - View Balance\n"
                 " 2. deposit     - Deposit Money\n"
                 " 3. withdraw    - Withdraw Money\n"
                 " 4. transfer    - Transfer Funds\n"
                 " 5. myloans     - View My Loan Applications\n"
                 " 6. applyloan   - Apply for Loan\n"
                 " 7. changepass  - Change Password\n"
                 " 8. feedback    - Add Feedback\n"
                 " 9. history     - View Transaction History\n"
                 "10. logout      - Logout\n"
                 "11. exit        - Exit\n"
                 "--------------------------------------------\n"
                 "Enter command: ",
                 username);
        if (!safe_send_check(client_fd, msg))
        {
            // Socket closed (client disconnected)
            printf("❌ %s disconnected (socket closed).\n", username);
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        }

        memset(buffer, 0, MAX);
        if (read(client_fd, buffer, MAX) <= 0)
        {
            // Client disconnected (e.g., Ctrl+C)
            printf("❌ %s disconnected.\n", username);
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        }
        trimInput(buffer);
        if (strlen(buffer) == 0)
            continue;

        // ---- Commands ----
        if (strcmp(buffer, "1") == 0 || strcasecmp(buffer, "view") == 0)
        {
            float bal = getBalance(userId);
            snprintf(msg, sizeof(msg), "💰 Current Balance: ₹%.2f\n", bal);
            safe_send(client_fd, msg);
        }
        else if (strcmp(buffer, "2") == 0 || strcasecmp(buffer, "deposit") == 0)
        {
            safe_send(client_fd, "Enter amount to deposit: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            float amt = atof(buffer);
            if (amt <= 0)
            {
                safe_send(client_fd, "❌ Invalid amount.\n");
                continue;
            }
            float bal = getBalance(userId) + amt;
            updateBalance(userId, bal);
            appendTransaction(username, "deposit", amt, bal, userId);
            snprintf(msg, sizeof(msg), "✅ ₹%.2f deposited. New Balance: ₹%.2f\n", amt, bal);
            safe_send(client_fd, msg);
        }
        else if (strcmp(buffer, "3") == 0 || strcasecmp(buffer, "withdraw") == 0)
        {
            safe_send(client_fd, "Enter amount to withdraw: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            float amt = atof(buffer);
            float bal = getBalance(userId);
            if (amt <= 0 || amt > bal)
            {
                safe_send(client_fd, "❌ Invalid or insufficient balance.\n");
                continue;
            }
            bal -= amt;
            updateBalance(userId, bal);
            appendTransaction(username, "withdraw", amt, bal, userId);
            snprintf(msg, sizeof(msg),
                     "✅ ₹%.2f withdrawn. Remaining Balance: ₹%.2f\n",
                     amt, bal);
            safe_send(client_fd, msg);
        }
        else if (strcmp(buffer, "4") == 0 || strcasecmp(buffer, "transfer") == 0)
        {
            safe_send(client_fd, "Enter recipient ID: ");
            memset(buffer, 0, MAX);
            if (safe_read_line(client_fd, buffer, MAX) <= 0)
            {
                // Client disconnected
                printf("❌ %s disconnected.\n", username);
                char userIdStr[32];
                snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
                removeSession(userIdStr);
                return; // handle disconnect or invalid input
            }

            trimInput(buffer);
            int recipientId = atoi(buffer);
            if (recipientId <= 0)
            {
                safe_send(client_fd, "❌ Invalid recipient ID.\n");
                return;
            }

            safe_send(client_fd, "Enter amount to transfer: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            float amt = atof(buffer);
            if (amt <= 0)
            {
                safe_send(client_fd, "❌ Invalid transfer amount.\n");
                continue;
            }

            // --- Reuse buffer for confirmation ---
            safe_send(client_fd, "Confirm transfer? (yes/no): ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);

            // Now `buffer` holds the confirmation
            if (!(strcasecmp(buffer, "yes") == 0 || strcasecmp(buffer, "y") == 0))
            {
                safe_send(client_fd, "⚠️ Transfer cancelled.\n");
                continue;
            }

            float fromNew, toNew;
            char toUser[50];
            int ok = transferFunds(username, recipientId, amt, &fromNew, &toNew, toUser);
            if (!ok)
            {
                safe_send(client_fd, "❌ Transfer failed (invalid user or insufficient balance).\n");
                continue;
            }

            appendTransaction(username, "transfer_out", amt, fromNew, userId);
            appendTransaction(toUser, "transfer_in", amt, toNew, recipientId);
            snprintf(msg, sizeof(msg),
                     "✅ Transferred ₹%.2f to %s.\nYour new balance: ₹%.2f\n",
                     amt, toUser, fromNew);
            safe_send(client_fd, msg);
        }
        else if (strcmp(buffer, "5") == 0 || strcasecmp(buffer, "myloans") == 0)
        {
            showCustomerLoans(client_fd, username, userId);
            // Continue to next iteration to show menu immediately
            continue;
        }
        else if (strcmp(buffer, "6") == 0 || strcasecmp(buffer, "applyloan") == 0)
        {
            safe_send(client_fd, "Enter loan amount: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            float amt = atof(buffer);
            if (amt <= 0)
            {
                safe_send(client_fd, "❌ Invalid loan amount.\n");
                continue;
            }
            safe_send(client_fd, "Enter loan reason: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            if (strlen(buffer) == 0)
            {
                safe_send(client_fd, "❌ Loan reason cannot be empty.\n");
                continue;
            }
            if (applyForLoan(username, amt, buffer, userId))
                safe_send(client_fd, "✅ Loan application submitted successfully.\n");
            else
                safe_send(client_fd, "❌ Failed to submit loan application.\n");
        }
        else if (strcmp(buffer, "7") == 0 || strcasecmp(buffer, "changepass") == 0)
        {
            safe_send(client_fd, "Enter old password: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            char oldPass[50];
            strcpy(oldPass, buffer);
            safe_send(client_fd, "Enter new password: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            char newPass[50];
            strcpy(newPass, buffer);
            if (changePassword(username, oldPass, newPass))
                safe_send(client_fd, "✅ Password changed successfully.\n");
            else
                safe_send(client_fd, "❌ Incorrect old password.\n");
        }
        else if (strcmp(buffer, "8") == 0 || strcasecmp(buffer, "feedback") == 0)
        {
            safe_send(client_fd, "Enter your feedback: ");
            memset(buffer, 0, MAX);
            read(client_fd, buffer, MAX);
            trimInput(buffer);
            if (strlen(buffer) == 0)
            {
                safe_send(client_fd, "❌ Feedback cannot be empty.\n");
                continue;
            }
            if (submitFeedback(username, buffer, userId))
                safe_send(client_fd, "✅ Feedback submitted successfully.\n");
            else
                safe_send(client_fd, "❌ Could not submit feedback.\n");
        }
        else if (strcmp(buffer, "9") == 0 || strcasecmp(buffer, "history") == 0)
            showTransactionHistory(client_fd, userId);
        else if (strcmp(buffer, "10") == 0 || strcasecmp(buffer, "logout") == 0)
        {
            safe_send(client_fd, "👋 Logged out successfully. Returning to login page...\n");
            char userIdStr[32];
            snprintf(userIdStr, sizeof(userIdStr), "%d", userId);
            removeSession(userIdStr);
            return;
        }
        else if (strcmp(buffer, "11") == 0 || strcasecmp(buffer, "exit") == 0)
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
