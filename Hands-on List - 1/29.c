/*
============================================================================
Name : 29.c
Author : Shikhar Mutta
Description : Write a program to get scheduling policy and modify the scheduling policy (SCHED_FIFO, SCHED_RR).
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

int main()
{
    pid_t pid = getpid();
    int policy;
    struct sched_param param;

    policy = sched_getscheduler(pid);
    if (policy == -1)
    {
        perror("sched_getscheduler failed");
        return 1;
    }

    switch (policy)
    {
    case SCHED_OTHER:
        printf("Current policy: SCHED_OTHER\n");
        break;
    case SCHED_FIFO:
        printf("Current policy: SCHED_FIFO\n");
        break;
    case SCHED_RR:
        printf("Current policy: SCHED_RR\n");
        break;
    default:
        printf("Current policy: Unknown\n");
    }

    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1)
    {
        perror("sched_setscheduler failed");
        return 1;
    }

    printf("Scheduling policy changed to SCHED_FIFO with priority %d\n", param.sched_priority);

    policy = sched_getscheduler(pid);
    switch (policy)
    {
    case SCHED_OTHER:
        printf("Updated policy: SCHED_OTHER\n");
        break;
    case SCHED_FIFO:
        printf("Updated policy: SCHED_FIFO\n");
        break;
    case SCHED_RR:
        printf("Updated policy: SCHED_RR\n");
        break;
    default:
        printf("Updated policy: Unknown\n");
    }

    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P29$ gcc 29.c -o 29

Run as root (required for real-time policies):
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P29$ sudo ./29
[sudo] password for shikhar: 
Current policy: SCHED_OTHER
Scheduling policy changed to SCHED_FIFO with priority 99
Updated policy: SCHED_FIFO
Sample Output
Current policy: SCHED_OTHER
Scheduling policy changed to SCHED_FIFO with priority 99
Updated policy: SCHED_FIFO

============================================================================
*/

