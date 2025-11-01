#ifndef MANAGER_H
#define MANAGER_H

#include "../include/common.h"

// Main entry point for manager
void handleManagerMenu(int client_fd, const int userId, const char *displayName);

// Core functions
int changeManagerPassword(int userId, const char *oldPass, const char *newPass);
int setCustomerStatus(int custId, const char *newStatus);
void showAllLoanApplications(int client_fd);
int assignLoanToEmployee(const char *loan_id, int employeeId);
void viewFeedback(int client_fd);

#endif
