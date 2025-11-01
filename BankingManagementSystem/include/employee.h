#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "../include/common.h"

void handleEmployeeMenu(int client_fd, const int userId, const char *displayName);

int addCustomer(const char *uname, const char *pass, float initial_balance);
int modifyCustomerById(int cust_id, float new_balance);
int modifyCustomerPasswordById(int cust_id, const char *newpass);

void showEmployeeLoans(int client_fd, int emp_id);
int processLoan(const char *loan_id, const char *decision, int employee_id);


void viewCustomerTransactions(int client_fd, int customerId);
int changePassword(const char *username, const char *oldPass, const char *newPass);

#endif // EMPLOYEE_H
