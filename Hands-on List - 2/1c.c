/*
============================================================================
Name : 1c.c
Author : Shikhar Mutta
Description : Write a separate program (for each time domain) to set a interval timer in 10sec and
              10micro second
                     c. ITIMER_PR
Date: 17th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

void handler(int signum)
{
    printf("ITIMER_PROF: Timer expired!\n");
}

int main()
{
    struct itimerval timer;

    signal(SIGPROF, handler);

    timer.it_value.tv_sec = 10;
    timer.it_value.tv_usec = 10;

    timer.it_interval.tv_sec = 10;
    timer.it_interval.tv_usec = 10;

    if (setitimer(ITIMER_PROF, &timer, NULL) == -1)
    {
        perror("setitimer");
        exit(1);
    }

    while (1)
    {
        for (volatile long i = 0; i < 100000000; i++)
            ;
    }
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P1$ gcc 1c.c -o 1c
2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P1$ ./1c
ITIMER_PROF: Timer expired!
ITIMER_PROF: Timer expired!

============================================================================
*/
