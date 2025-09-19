/*
============================================================================
Name : 21.c
Author : Shikhar Mutta
Description : Write a program, call fork and print the parent and child process id.
Date: 24th Aug, 2025.
============================================================================
*/

#include <stdio.h>
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
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
    }
    else
    {

        printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), pid);
    }
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P21$ gcc 21.c -o 21

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P21$ ./21

=>output :
Parent Process: PID = 9958, Child PID = 9959
Child Process: PID = 9959, Parent PID = 9958

============================================================================
*/
