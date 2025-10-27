/*
============================================================================
Name : 8.c
Author : Shikhar Mutta
Description : Write a separate program using signal system call to catch the following signals.
                    a. SIGSEGV
                    b. SIGINT
                    c. SIGFPE
                    d. SIGALRM (use alarm system call)
                    e. SIGALRM (use setitimer system call)
                    f. SIGVTALRM (use setitimer system call)
                    g. SIGPROF (use setitimer system call
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

void handler(int signum)
{
    const char *name = strsignal(signum);
    if (name == NULL)
        name = "Unknown signal";
    printf("\n>>> Caught signal %d: %s\n", signum, name);
    fflush(stdout);

    if (signum == SIGSEGV || signum == SIGFPE)
    {
        printf("Exiting due to fatal signal %d.\n", signum);
        _exit(128 + signum);
    }
}

void trigger_sigsegv(void)
{
    volatile int *p = NULL;
    printf("About to dereference NULL pointer -> SIGSEGV should be raised.\n");
    fflush(stdout);
    *p = 42;
}

void trigger_sigfpe(void)
{
    volatile int zero = 0;
    volatile int x = 1;
    printf("About to divide by zero -> SIGFPE should be raised.\n");
    fflush(stdout);
    volatile int y = x / zero;
    (void)y;
}

void test_alarm_syscall(void)
{
    printf("Installing handler for SIGALRM and calling alarm(3). You should see SIGALRM in ~3s.\n");
    fflush(stdout);
    alarm(3);
    pause();
    printf("Returned from pause() after SIGALRM.\n");
}

void test_setitimer_real(void)
{
    struct itimerval tv;
    printf("Setting ITIMER_REAL for a single expiration in 2 seconds (delivers SIGALRM)\n");
    fflush(stdout);

    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 0;
    tv.it_value.tv_sec = 2;
    tv.it_value.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &tv, NULL) != 0)
    {
        perror("setitimer(ITIMER_REAL)");
        return;
    }

    pause();
    printf("Returned from pause() after ITIMER_REAL(SIGALRM).\n");
}

void test_setitimer_vitual(void)
{
    struct itimerval tv;
    printf("Setting ITIMER_VIRTUAL for 1 second. This sends SIGVTALRM after 1s of *user CPU time*.\n");
    printf("To trigger it, the program will spin for ~2 seconds of busy CPU.\n");
    fflush(stdout);

    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 0;
    tv.it_value.tv_sec = 1;
    tv.it_value.tv_usec = 0;

    if (setitimer(ITIMER_VIRTUAL, &tv, NULL) != 0)
    {
        perror("setitimer(ITIMER_VIRTUAL)");
        return;
    }

    volatile unsigned long i = 0;
    while (1)
    {
        i += 1;
        if ((i & 0x0FFFFFFF) == 0)
        {
        }
    }
}

void test_setitimer_prof(void)
{
    struct itimerval tv;
    printf("Setting ITIMER_PROF for 1 second. This sends SIGPROF after 1s of CPU (user+sys).\n");
    printf("We'll perform CPU-heavy work for a short while to trigger it.\n");
    fflush(stdout);

    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = 0;
    tv.it_value.tv_sec = 1;
    tv.it_value.tv_usec = 0;

    if (setitimer(ITIMER_PROF, &tv, NULL) != 0)
    {
        perror("setitimer(ITIMER_PROF)");
        return;
    }

    volatile unsigned long i = 0;
    while (1)
    {
        getpid();
        i++;
    }
}

int main(void)
{
    char choice[8];

    signal(SIGSEGV, handler);
    signal(SIGINT, handler);
    signal(SIGFPE, handler);
    signal(SIGALRM, handler);
    signal(SIGVTALRM, handler);
    signal(SIGPROF, handler);

    printf("Signal-catching demo (using signal()). Choose a test to run:\n");
    printf(" a) SIGSEGV (segmentation fault)\n");
    printf(" b) SIGINT  (press Ctrl-C)\n");
    printf(" c) SIGFPE  (divide by zero)\n");
    printf(" d) SIGALRM (use alarm())\n");
    printf(" e) SIGALRM (use setitimer ITIMER_REAL)\n");
    printf(" f) SIGVTALRM (use setitimer ITIMER_VIRTUAL)\n");
    printf(" g) SIGPROF (use setitimer ITIMER_PROF)\n");
    printf(" q) Quit\n");
    printf("Enter choice: ");
    fflush(stdout);

    if (!fgets(choice, sizeof(choice), stdin))
        return 0;

    switch (choice[0])
    {
    case 'a':
    case 'A':
        printf("Selected SIGSEGV test.\n");
        trigger_sigsegv();
        break;
    case 'b':
    case 'B':
        printf("Selected SIGINT test.\n");
        printf("Now: press Ctrl-C (SIGINT) within 10 seconds. Sleeping 10s...\n");
        fflush(stdout);
        sleep(10);
        printf("Done sleeping. If you pressed Ctrl-C it should have been caught.\n");
        break;
    case 'c':
    case 'C':
        printf("Selected SIGFPE test.\n");
        trigger_sigfpe();
        break;
    case 'd':
    case 'D':
        printf("Selected SIGALRM via alarm().\n");
        test_alarm_syscall();
        break;
    case 'e':
    case 'E':
        printf("Selected SIGALRM via setitimer(ITIMER_REAL).\n");
        test_setitimer_real();
        break;
    case 'f':
    case 'F':
        printf("Selected SIGVTALRM via setitimer(ITIMER_VIRTUAL).\n");
        test_setitimer_vitual();
        break;
    case 'g':
    case 'G':
        printf("Selected SIGPROF via setitimer(ITIMER_PROF).\n");
        test_setitimer_prof();
        break;
    case 'q':
    case 'Q':
    default:
        printf("Quitting.\n");
        break;
    }

    printf("Program finished (or returned to menu). Bye.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P8$ gcc 8.c -o 8

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P8$ ./8
Signal-catching demo (using signal()). Choose a test to run:
 a) SIGSEGV (segmentation fault)
 b) SIGINT  (press Ctrl-C)
 c) SIGFPE  (divide by zero)
 d) SIGALRM (use alarm())
 e) SIGALRM (use setitimer ITIMER_REAL)
 f) SIGVTALRM (use setitimer ITIMER_VIRTUAL)
 g) SIGPROF (use setitimer ITIMER_PROF)
 q) Quit

------------------------------------------------------------------
Enter choice: a
Selected SIGSEGV test.
About to dereference NULL pointer -> SIGSEGV should be raised.

>>> Caught signal 11: Segmentation fault
Exiting due to fatal signal 11.
------------------------------------------------------------------
Enter choice: b
Selected SIGINT test.
Now: press Ctrl-C (SIGINT) within 10 seconds. Sleeping 10s...
^C
>>> Caught signal 2: Interrupt
Done sleeping. If you pressed Ctrl-C it should have been caught.
Program finished (or returned to menu). Bye.
------------------------------------------------------------------
Enter choice: c
Selected SIGFPE test.
About to divide by zero -> SIGFPE should be raised.

>>> Caught signal 8: Floating point exception
Exiting due to fatal signal 8.
------------------------------------------------------------------
Enter choice: d
Selected SIGALRM via alarm().
Installing handler for SIGALRM and calling alarm(3). You should see SIGALRM in ~3s.

>>> Caught signal 14: Alarm clock
Returned from pause() after SIGALRM.
Program finished (or returned to menu). Bye.
------------------------------------------------------------------
Enter choice: e
Selected SIGALRM via setitimer(ITIMER_REAL).
Setting ITIMER_REAL for a single expiration in 2 seconds (delivers SIGALRM)

>>> Caught signal 14: Alarm clock
Returned from pause() after ITIMER_REAL(SIGALRM).
Program finished (or returned to menu). Bye.
------------------------------------------------------------------
Enter choice: f
Selected SIGVTALRM via setitimer(ITIMER_VIRTUAL).
Setting ITIMER_VIRTUAL for 1 second. This sends SIGVTALRM after 1s of *user CPU time*.
To trigger it, the program will spin for ~2 seconds of busy CPU.

>>> Caught signal 26: Virtual timer expired
------------------------------------------------------------------
Enter choice: g
Selected SIGPROF via setitimer(ITIMER_PROF).
Setting ITIMER_PROF for 1 second. This sends SIGPROF after 1s of CPU (user+sys).
We'll perform CPU-heavy work for a short while to trigger it.

>>> Caught signal 27: Profiling timer expired
------------------------------------------------------------------
Enter choice: q
Quitting.
Program finished (or returned to menu). Bye.

============================================================================
*/
