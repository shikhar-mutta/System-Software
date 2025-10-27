/*
============================================================================
Name : 12.c
Author : Shikhar Mutta
Description : Write a program to create an orphan process. Use kill system call to send SIGKILL signal to the parent process from the child process.
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        printf("Child: my PID = %d, parent PID = %d\n", getpid(), getppid());
        printf("Child: sending SIGKILL to parent (PID %d)...\n", getppid());
        if (kill(getppid(), SIGKILL) == -1)
        {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        sleep(2);
        printf("Child: new parent PID = %d (should be 1 = init/systemd)\n", getppid());
        for (int i = 0; i < 5; i++)
        {
            printf("Child: still alive, iteration %d\n", i + 1);
            sleep(1);
        }
        printf("Child: exiting.\n");
    }
    else
    {
        printf("Parent: my PID = %d, child PID = %d\n", getpid(), pid);
        while (1)
        {
            sleep(1);
        }
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P12$ gcc 12.c -o 12

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P12$ ./12
Parent: my PID = 5190, child PID = 5191
Child: my PID = 5191, parent PID = 5190
Child: sending SIGKILL to parent (PID 5190)...
Killed

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P12$ Child: new parent PID = 2357 (should be 1 = init/systemd)
Child: still alive, iteration 1
Child: still alive, iteration 2
Child: still alive, iteration 3
Child: still alive, iteration 4
Child: still alive, iteration 5
Child: exiting.

============================================================================
*/
