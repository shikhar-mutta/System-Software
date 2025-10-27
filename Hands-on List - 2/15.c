/*
============================================================================
Name : 15.c
Author : Shikhar Mutta
Description : Write a simple program to send some data from parent to the child process.
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
    const char *message = "Hello Child, this is Parent!";

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
        printf("Parent: sending message to child...\n");
        write(fd[1], message, strlen(message) + 1);
        close(fd[1]);
    }
    else
    {
        close(fd[1]);
        printf("Child: waiting for message from parent...\n");
        ssize_t n = read(fd[0], buffer, sizeof(buffer));
        if (n > 0)
        {
            printf("Child: received message: \"%s\"\n", buffer);
        }
        else
        {
            printf("Child: no data received.\n");
        }
        close(fd[0]);
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P15$ gcc 15.c -o 15

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P15$ ./15
Parent: sending message to child...
Child: waiting for message from parent...
Child: received message: "Hello Child, this is Parent!"

============================================================================
*/
