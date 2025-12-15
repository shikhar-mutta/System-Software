#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "../include/common.h"
#include "../include/structures.h"
#include "../include/session.h"

// ---------------------
// Customer Menu Handler
// ---------------------
/**
 * @brief Starts interactive menu for a logged-in customer.
 *
 * @param client_fd Connected socket descriptor
 * @param userId    Logged-in customer's user ID
 * @param displayName Customer's display name
 */
void handleCustomerMenu(int client_fd, const int userId, const char *displayName);

// ---------------------
// Core Banking Actions
// ---------------------
float getBalance(const int userId);
int updateBalance(const int userId, float newBalance);
int transferFunds(const char *fromUser, const int toUserId, float amount, float *fromNewBal, float *toNewBal, char *toUser);
int applyForLoan(const char *username, float amount, const char *reason, int userId);
int changePassword(const char *username, const char *oldPass, const char *newPass);
int submitFeedback(const char *username, const char *message, int userId);

// ---------------------
// Transaction Management
// ---------------------
int appendTransaction(const char *username, const char *type, float amount, float balance_after, int userId);
void showTransactionHistory(int client_fd, int userId);
void showCustomerLoans(int client_fd, const char *username, int userId);

#endif // CUSTOMER_H
