#include "../include/session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SESSION_FILE "../data/sessions.txt"
#define CUSTOMER_FILE "../data/customers.txt"
#define EMPLOYEE_FILE "../data/employees.txt"
#define TEMP_SESSION_FILE "../data/temp_sessions.txt"

// =========================================================
// 🟢 Check if a User (by ID) is already logged in
// =========================================================
int isUserLoggedIn(const char *userId)
{
    FILE *fp = fopen(SESSION_FILE, "r");
    if (!fp)
        return 0;

    char id[64], uname[64], role[32];
    int loggedIn = 0;

    while (fscanf(fp, "%63s %63s %31s", id, uname, role) == 3)
    {
        if (strcmp(id, userId) == 0)
        {
            loggedIn = 1;
            break;
        }
    }

    fclose(fp);
    return loggedIn;
}

// =========================================================
// 🟢 Add New Session Entry (ID + Username + Role)
// =========================================================
int addSession(const char *userId, const char *role)
{
    if (!userId || !role)
        return 0;

    FILE *fp = fopen(SESSION_FILE, "a");
    if (!fp)
        return 0;

    char name[64] = "Unknown";
    int id = atoi(userId);

    // Determine source file (customer or employee)
    FILE *db = NULL;
    if (strcasecmp(role, "customer") == 0)
        db = fopen(CUSTOMER_FILE, "r");
    else
        db = fopen(EMPLOYEE_FILE, "r");

    if (db)
    {
        int db_id;
        char uname[64], pass[64], extra[64];
        float bal;

        if (strcasecmp(role, "customer") == 0)
        {
            // Format: ID Name Pass Balance Status
            while (fscanf(db, "%d %63s %63s %f %63s", &db_id, uname, pass, &bal, extra) == 5)
            {
                if (db_id == id)
                {
                    strcpy(name, uname);
                    break;
                }
            }
        }
        else
        {
            // Format: ID Name Pass Role
            while (fscanf(db, "%d %63s %63s %63s", &db_id, uname, pass, extra) == 4)
            {
                if (db_id == id)
                {
                    strcpy(name, uname);
                    break;
                }
            }
        }

        fclose(db);
    }

    fprintf(fp, "%s %s %s\n", userId, name, role);
    fclose(fp);
    return 1;
}

// =========================================================
// 🟢 Remove Session (When User Logs Out)
// =========================================================
int removeSession(const char *userId)
{
    FILE *fp = fopen(SESSION_FILE, "r");
    FILE *temp = fopen(TEMP_SESSION_FILE, "w");
    if (!fp || !temp)
    {
        if (fp)
            fclose(fp);
        if (temp)
            fclose(temp);
        return 0;
    }

    char id[64], uname[64], role[32];
    while (fscanf(fp, "%63s %63s %31s", id, uname, role) == 3)
    {
        if (strcmp(id, userId) != 0)
            fprintf(temp, "%s %s %s\n", id, uname, role);
    }

    fclose(fp);
    fclose(temp);
    remove(SESSION_FILE);
    rename(TEMP_SESSION_FILE, SESSION_FILE);
    return 1;
}

// =========================================================
// 🧹 Clear All Sessions on Server Restart
// =========================================================
void clearAllSessions()
{
    FILE *fp = fopen(SESSION_FILE, "w");
    if (fp)
        fclose(fp);
}
