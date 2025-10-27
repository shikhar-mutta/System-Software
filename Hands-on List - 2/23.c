/*
============================================================================
Name : 23.c
Author : Shikhar Mutta
Description : Write a program to print the maximum number of files can be opened within a process and size of a pipe (circular buffer).
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>

int main(void)
{
    struct rlimit rl;
    int pipefd[2];

    if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        perror("getrlimit");
        return 1;
    }

    printf("Maximum number of open files (soft limit): %ld\n", (long)rl.rlim_cur);
    printf("Maximum number of open files (hard limit): %ld\n", (long)rl.rlim_max);

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

#ifdef F_GETPIPE_SZ
    int size = fcntl(pipefd[0], F_GETPIPE_SZ);
    if (size == -1)
    {
        perror("fcntl F_GETPIPE_SZ");
    }
    else
    {
        printf("Default pipe (circular buffer) size: %d bytes\n", size);
    }
#else
    printf("F_GETPIPE_SZ not supported on this system.\n");
#endif

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P23$ gcc 23.c -o 23 

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P23$ ./23
Maximum number of open files (soft limit): 1024
Maximum number of open files (hard limit): 1048576
F_GETPIPE_SZ not supported on this system.

============================================================================
*/
