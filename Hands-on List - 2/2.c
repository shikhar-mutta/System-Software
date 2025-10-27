/*
============================================================================
Name : 2.c
Author : Shikhar Mutta
Description : Write a program to print the system resource limits. Use getrlimit system call.
Date: 17th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

void print_limit(int resource, const char *name)
{
    struct rlimit limit;
    if (getrlimit(resource, &limit) == -1)
    {
        perror("getrlimit");
        return;
    }

    printf("%-15s : ", name);

    if (limit.rlim_cur == RLIM_INFINITY)
        printf("Soft = unlimited, ");
    else
        printf("Soft = %ld, ", (long)limit.rlim_cur);

    if (limit.rlim_max == RLIM_INFINITY)
        printf("Hard = unlimited\n");
    else
        printf("Hard = %ld\n", (long)limit.rlim_max);
}

int main()
{
    print_limit(RLIMIT_CPU, "CPU time");
    print_limit(RLIMIT_FSIZE, "File size");
    print_limit(RLIMIT_DATA, "Data seg size");
    print_limit(RLIMIT_STACK, "Stack size");
    print_limit(RLIMIT_CORE, "Core file size");
    print_limit(RLIMIT_RSS, "Resident set");
    print_limit(RLIMIT_NOFILE, "Open files");
    print_limit(RLIMIT_AS, "Address space");
    print_limit(RLIMIT_NPROC, "Processes");
    print_limit(RLIMIT_MEMLOCK, "Locked memory");

#ifdef RLIMIT_MSGQUEUE
    print_limit(RLIMIT_MSGQUEUE, "POSIX msgq size");
#endif
#ifdef RLIMIT_NICE
    print_limit(RLIMIT_NICE, "Nice value");
#endif
#ifdef RLIMIT_RTPRIO
    print_limit(RLIMIT_RTPRIO, "Realtime prio");
#endif

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P2$ gcc 2.c -o 2

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P2$ ./2
CPU time        : Soft = unlimited, Hard = unlimited
File size       : Soft = unlimited, Hard = unlimited
Data seg size   : Soft = unlimited, Hard = unlimited
Stack size      : Soft = 8388608, Hard = unlimited
Core file size  : Soft = 0, Hard = unlimited
Resident set    : Soft = unlimited, Hard = unlimited
Open files      : Soft = 1024, Hard = 1048576
Address space   : Soft = unlimited, Hard = unlimited
Processes       : Soft = 30280, Hard = 30280
Locked memory   : Soft = 1011900416, Hard = 1011900416
POSIX msgq size : Soft = 819200, Hard = 819200
Nice value      : Soft = 0, Hard = 0
Realtime prio   : Soft = 0, Hard = 0


============================================================================
*/
