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
 * @param username  Logged-in customer's username
 */
void handleCustomerMenu(int client_fd, const int userId, const char *displayName);

// ---------------------
// Core Banking Actions
// ---------------------
float getBalance(const char *username);
int updateBalance(const char *username, float newBalance);
int transferFunds(const char *fromUser, const char *toUser, float amount, float *fromNewBal, float *toNewBal);
int applyForLoan(const char *username, float amount, const char *reason);
int changePassword(const char *username, const char *oldPass, const char *newPass);
int submitFeedback(const char *username, const char *message);

// ---------------------
// Transaction Management
// ---------------------
int appendTransaction(const char *username, const char *type, float amount, float balance_after);
void showTransactionHistory(int client_fd, const char *username);

#endif // CUSTOMER_H
