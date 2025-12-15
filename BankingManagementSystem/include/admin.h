#ifndef ADMIN_H
#define ADMIN_H

#include "common.h"

// ---------------------
// Employee Management
// ---------------------
int addEmployee(const char *username, const char *password, const char *role);
int modifyUserDetailsById(const char *filename, int userId, const char *newPass);
int changeUserRole(int client_fd, int userId, const char *newRole);

// ---------------------
// Password Management
// ---------------------
int changeAdminPassword(int adminId, const char *oldPass, const char *newPass);

// ---------------------
// Admin Menu Handler
// ---------------------
void handleAdminMenu(int client_fd, const int userId, const char *displayName);

#endif
