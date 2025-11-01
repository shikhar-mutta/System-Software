#ifndef SESSION_H
#define SESSION_H

#include "common.h"

typedef struct {
    char username[50];
    char role[20];
    int active;
} Session;

int addSession(const char *username, const char *role);
int removeSession(const char *username);
int isUserLoggedIn(const char *username);
void clearAllSessions();

#endif
