#ifndef STRUCTURES_H
#define STRUCTURES_H

#define NAME_LEN 50
#define PASS_LEN 20
#define STATUS_LEN 10

typedef struct {
    int id;
    char name[NAME_LEN];
    char password[PASS_LEN];
    float balance;
    char status[STATUS_LEN];
} Customer;

typedef struct {
    int id;
    char name[NAME_LEN];
    char password[PASS_LEN];
    char role[20]; // employee / manager / admin
} Employee;

typedef struct {
    int id;
    int customer_id;
    float amount;
    char status[20];
    int assigned_to;
} Loan;

int validateCustomerLogin(const char *username, const char *password);
int validateEmployeeLogin(const char *empId, const char *password, char *outRole, const char *userType);

#endif
