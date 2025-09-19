/*
============================================================================
Name : 25.c
Author : Shikhar Mutta
Description : Write a program to create three child processes. The parent should wait for a particular child (use waitpid system call).
Date: 24th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid1, pid2, pid3;
    pid_t waited_pid;
    pid1 = fork();
    if (pid1 == 0)
    {
        printf("Child 1 (PID = %d) running\n", getpid());
        sleep(2);
        printf("Child 1 (PID = %d) exiting\n", getpid());
        exit(10);
    }
    pid2 = fork();
    if (pid2 == 0)
    {
        printf("Child 2 (PID = %d) running\n", getpid());
        sleep(4);
        printf("Child 2 (PID = %d) exiting\n", getpid());
        exit(20);
    }
    pid3 = fork();
    if (pid3 == 0)
    {
        printf("Child 3 (PID = %d) running\n", getpid());
        sleep(6);
        printf("Child 3 (PID = %d) exiting\n", getpid());
        exit(30);
    }
    int status;
    printf("Parent waiting for Child 2 (PID = %d)\n", pid2);
    waited_pid = waitpid(pid2, &status, 0);
    if (waited_pid > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Parent: Child 2 exited with status %d\n", WEXITSTATUS(status));
        }
    }
    wait(NULL);
    wait(NULL);
    printf("Parent (PID = %d) exiting\n", getpid());
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P25$ gcc 25.c -o 25

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P25$ ./25

Output : 
Child 1 (PID = 12588) running
Parent waiting for Child 2 (PID = 12589)
Child 2 (PID = 12589) running
Child 3 (PID = 12590) running
Child 1 (PID = 12588) exiting
Child 2 (PID = 12589) exiting
Parent: Child 2 exited with status 20
Child 3 (PID = 12590) exiting
Parent (PID = 12587) exiting

============================================================================
*/
