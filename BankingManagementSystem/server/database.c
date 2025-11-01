#include "../include/common.h"
#include "../include/structures.h"

int validateCustomerLogin(const char *custId, const char *password);
int validateEmployeeLogin(const char *empId, const char *password, char *outRole, const char *userType);

int validateCustomerLogin(const char *custId, const char *password)
{
    if (!custId || !password)
        return 0;

    int fd = open("../data/customers.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("customers.txt open failed");
        return 0;
    }

    // Apply shared read lock
    struct flock lock = {0};
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl lock failed");
        close(fd);
        return 0;
    }

    FILE *fp = fdopen(fd, "r");
    if (!fp)
    {
        close(fd);
        return 0;
    }

    int id;
    char uname[64], pass[64], status[16];
    float bal;
    int result = 0; // 0 = not found, 1 = active match, -1 = inactive match
    int inputId = atoi(custId);

    // Read each line safely
    while (fscanf(fp, "%d %63s %63s %f %15s", &id, uname, pass, &bal, status) == 5)
    {
        if (id == inputId && strcmp(pass, password) == 0)
        {
            if (strcasecmp(status, "active") == 0)
                result = 1; // ✅ Valid & active
            else
                result = -1; // 🚫 Inactive
            break;
        }
    }

    // Unlock and close file
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(fp);
    close(fd);

    return result;
}

int validateEmployeeLogin(const char *empId, const char *password, char *outRole, const char *userType)
{
    if (!empId || !password)
        return 0;

    int fd = open("../data/employees.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("employees.txt open failed");
        return 0;
    }

    struct flock lock = {0};
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    fcntl(fd, F_SETLKW, &lock);

    FILE *fp = fdopen(fd, "r");
    if (!fp)
    {
        close(fd);
        return 0;
    }

    int id;
    char uname[64], pass[64], role[32];
    int result = 0;
    int inputId = atoi(empId);

    while (fscanf(fp, "%d %63s %63s %31s", &id, uname, pass, role) == 4)
    {
        if (id == inputId && strcmp(pass, password) == 0 && strcmp(role, userType) == 0)
        {
            strncpy(outRole, role, 31);
            outRole[31] = '\0';
            result = 1;
            break;
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    fclose(fp);
    close(fd);
    return result;
}
