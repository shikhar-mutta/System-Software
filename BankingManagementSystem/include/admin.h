#ifndef ADMIN_H
#define ADMIN_H

#include "common.h"

int addEmployee(const char *username, const char *password, const char *role);
int modifyUserDetails(const char *filename, const char *username, const char *newPass);
int changeUserRole(int client_fd, int userId, const char *newRole);
int changeAdminPassword(int adminId, const char *oldPass, const char *newPass);
void handleAdminMenu(int client_fd, const int userId, const char *displayName);

#endif
