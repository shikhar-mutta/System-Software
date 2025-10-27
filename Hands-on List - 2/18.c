/*
============================================================================
Name : 18.c
Author : Shikhar Mutta
Description : Write a program to find out total number of directories on the pwd. execute ls -l | grep ^d | wc ? Use only dup2
                -> Here used Comand : ls -l | grep ^-rwx | wc
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int p1[2];
    int p2[2];
    pid_t pid;

    if (pipe(p1) == -1)
    {
        perror("pipe p1");
        exit(EXIT_FAILURE);
    }
    if (pipe(p2) == -1)
    {
        perror("pipe p2");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        close(p1[0]);
        if (dup2(p1[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 ls");
            _exit(127);
        }
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp ls");
        _exit(127);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        close(p1[1]);
        if (dup2(p1[0], STDIN_FILENO) == -1)
        {
            perror("dup2 grep stdin");
            _exit(127);
        }
        close(p1[0]);

        close(p2[0]);
        if (dup2(p2[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 grep stdout");
            _exit(127);
        }
        close(p2[1]);

        execlp("grep", "grep", "^-rwx", (char *)NULL);
        perror("execlp grep");
        _exit(127);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        close(p2[1]);
        if (dup2(p2[0], STDIN_FILENO) == -1)
        {
            perror("dup2 wc");
            _exit(127);
        }
        close(p2[0]);

        close(p1[0]);
        close(p1[1]);

        execlp("wc", "wc", (char *)NULL);
        perror("execlp wc");
        _exit(127);
    }

    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);

    while (wait(NULL) > 0)
    {
    }

    return EXIT_SUCCESS;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P18$ gcc 18.c -o 18

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P18$ ./18
      1       9      51


Vrification : 
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P18$ ls -l
total 20
-rwxrwxr-x 1 shikhar shikhar 16328 Sep 19 12:22 18
-rw-rw-r-- 1 shikhar shikhar  2699 Sep 19 12:23 18.c
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P18$ ls -l | grep ^-rwx 
-rwxrwxr-x 1 shikhar shikhar 16328 Sep 19 12:22 18
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P18$ ls -l | grep ^-rwx | wc 
      1       9      51

============================================================================
*/
