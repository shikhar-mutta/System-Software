/*
============================================================================
Name : 19.c
Author : Shikhar Mutta
Description : Write a program to find out time taken to execute getpid system call. Use time stamp counter.
Date: 23th Aug, 2025.
============================================================================
*/

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>

static __inline__ uint64_t rdtsc(void)
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

double get_cpu_frequency()
{
    struct timespec ts_start, ts_end;
    uint64_t start, end;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    start = rdtsc();
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    end = rdtsc();
    double elapsed_sec = (ts_end.tv_sec - ts_start.tv_sec) +
                         (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9;
    return (end - start) / elapsed_sec;
}

int main()
{
    double cpu_hz = get_cpu_frequency();
    double cpu_ghz = cpu_hz / 1e9;
    uint64_t start = rdtsc();
    pid_t pid = getpid();
    uint64_t end = rdtsc();
    uint64_t cycles = end - start;
    double seconds = cycles / cpu_hz;
    printf("CPU frequency: %.2f GHz\n", cpu_ghz);
    printf("PID: %d\n", pid);
    printf("getpid() took: %lu cycles ≈ %.9f seconds\n",
           (unsigned long)cycles, seconds);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P19$ gcc 19.c -o 19

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P19$ ./19

Output (example, will vary by CPU speed):
CPU frequency: 1.19 GHz
PID: 16415
getpid() took: 3038 cycles ≈ 0.000002552 seconds

============================================================================
*/
