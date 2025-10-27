/*
============================================================================
Name : 3.c
Author : Shikhar Mutta
Description : Write a program to set (any one) system resource limit. Use setrlimit system call.
Date: 17th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

int main()
{
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) == -1)
    {
        perror("getrlimit");
        exit(1);
    }

    printf("Open files limit before change: Soft = %ld, Hard = %ld\n",
           (long)limit.rlim_cur, (long)limit.rlim_max);

    limit.rlim_cur = 2048;
    limit.rlim_max = limit.rlim_max;

    if (setrlimit(RLIMIT_NOFILE, &limit) == -1)
    {
        perror("setrlimit");
        exit(1);
    }

    if (getrlimit(RLIMIT_NOFILE, &limit) == -1)
    {
        perror("getrlimit");
        exit(1);
    }

    printf("Open files limit after change : Soft = %ld, Hard = %ld\n",
           (long)limit.rlim_cur, (long)limit.rlim_max);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P3$ gcc 3.c -o 3

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P3$ ./3
Open files limit before change: Soft = 1024, Hard = 1048576
Open files limit after change : Soft = 2048, Hard = 1048576

============================================================================
*/
