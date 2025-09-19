/*
============================================================================
Name : 28.c
Author : Shikhar Mutta
Description : Write a program to get maximum and minimum real time priority.
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <sched.h>

int main()
{
    int max_fifo, min_fifo;
    int max_rr, min_rr;
    max_fifo = sched_get_priority_max(SCHED_FIFO);
    min_fifo = sched_get_priority_min(SCHED_FIFO);
    max_rr = sched_get_priority_max(SCHED_RR);
    min_rr = sched_get_priority_min(SCHED_RR);
    printf("SCHED_FIFO: Max Priority = %d, Min Priority = %d\n", max_fifo, min_fifo);
    printf("SCHED_RR  : Max Priority = %d, Min Priority = %d\n", max_rr, min_rr);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P28$ gcc 28.c -o 28

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P28$ ./28
SCHED_FIFO: Max Priority = 99, Min Priority = 1
SCHED_RR  : Max Priority = 99, Min Priority = 1

============================================================================
*/

