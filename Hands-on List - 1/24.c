/*
============================================================================
Name : 24.c
Author : Shikhar Mutta
Description : Write a program to create an orphan process.
Date: 24th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        printf("Child process (PID = %d) running. Parent PID = %d\n", getpid(), getppid());
        sleep(10);
        printf("Child process (PID = %d) after parent exit. New Parent PID = %d\n", getpid(), getppid());
    }
    else
    {
        printf("Parent process (PID = %d) exiting...\n", getpid());
        exit(0);
    }
    return 0;
}


/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P24$ gcc 24.c -o 24

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P24$ ./24

Output :

Parent process (PID = 12101) exiting...
Child process (PID = 12102) running. Parent PID = 12101

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P24$ Child process (PID = 12102) after parent exit. New Parent PID = 2658

============================================================================
*/


