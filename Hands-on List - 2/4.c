/*
============================================================================
Name : 4.c
Author : Shikhar Mutta
Description : Write a program to measure how much time is taken to execute 100 getppid() system call. Use time stamp counter.
Date: 17th September 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <x86intrin.h>

double get_cpu_freq()
{
    unsigned long long start, end;
    start = __rdtsc();
    sleep(1);
    end = __rdtsc();
    return (double)(end - start);
}

int main()
{
    unsigned long long start_cycles, end_cycles;
    int i;

    double cpu_freq = get_cpu_freq();

    start_cycles = __rdtsc();
    for (i = 0; i < 100; i++)
        getppid();
    end_cycles = __rdtsc();

    unsigned long long total_cycles = end_cycles - start_cycles;
    double time_sec = total_cycles / cpu_freq;

    printf("Total cycles for 100 getppid() calls: %llu\n", total_cycles);
    printf("Time taken: %e seconds\n", time_sec);
    printf("Average cycles per getppid(): %llu\n", total_cycles / 100);
    printf("Average time per getppid(): %e seconds\n", time_sec / 100);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P4$ gcc 4.c -o 4

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P4$ ./4
Total cycles for 100 getppid() calls: 105743
Time taken: 8.881918e-05 seconds
Average cycles per getppid(): 1057
Average time per getppid(): 8.881918e-07 seconds

============================================================================
*/
