/*
============================================================================
Name : 14.c
Author : Shikhar Mutta
Description : Write a simple program to create a pipe, write to the pipe, read from pipe and display on the monitor.
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

int main(void)
{
    int fd[2];
    pid_t pid;
    char buffer[100];
    const char *msg = "Hello from parent via pipe!";

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        close(fd[0]);
        printf("Parent: writing to pipe...\n");
        write(fd[1], msg, strlen(msg) + 1);
        close(fd[1]);
    }
    else
    {
        close(fd[1]);
        printf("Child: reading from pipe...\n");
        ssize_t n = read(fd[0], buffer, sizeof(buffer));
        if (n > 0)
        {
            printf("Child: got message: \"%s\"\n", buffer);
        }
        else
        {
            printf("Child: no data read from pipe\n");
        }
        close(fd[0]);
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P14$ gcc 14.c -o 14

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P14$ ./14
Parent: writing to pipe...
Child: reading from pipe...
Child: got message: "Hello from parent via pipe!"


============================================================================
*/
