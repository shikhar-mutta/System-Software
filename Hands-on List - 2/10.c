/*
============================================================================
Name : 10.c
Author : Shikhar Mutta
Description : Write a separate program using sigaction system call to catch the following signals.
                        a. SIGSEGV
                        b. SIGINT
                        c. SIGFPE
Date: 18th September 2025.
============================================================================
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void print_siginfo(int sig, siginfo_t *info)
{
    if (!info)
        return;
    printf("  siginfo: si_signo=%d, si_errno=%d, si_code=%d\n",
           info->si_signo, info->si_errno, info->si_code);

    switch (sig)
    {
    case SIGSEGV:
        printf("  SIGSEGV faulting address: %p\n", info->si_addr);
        break;
    case SIGFPE:
        printf("  SIGFPE: trap code (si_code) = %d\n", info->si_code);
        break;
    default:
        break;
    }
}

static void handler(int signum, siginfo_t *info, void *ucontext)
{
    (void)ucontext;
    const char *name = strsignal(signum);
    if (name == NULL)
        name = "Unknown";

    printf("\n*** Caught signal %d: %s\n", signum, name);
    print_siginfo(signum, info);

    if (signum == SIGSEGV || signum == SIGFPE)
    {
        printf("Exiting due to fatal signal %d.\n", signum);
        _exit(128 + signum);
    }

}

static void trigger_sigsegv(void)
{
    volatile int *p = NULL;
    printf("Triggering SIGSEGV by writing to NULL pointer...\n");
    fflush(stdout);
    *p = 1; 
}

static void trigger_sigfpe(void)
{
    volatile int zero = 0;
    volatile int x = 1;
    printf("Triggering SIGFPE by dividing by zero...\n");
    fflush(stdout);
    volatile int y = x / zero;
    (void)y;
}

static void install_handlers(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        perror("sigaction(SIGSEGV)");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGFPE, &sa, NULL) == -1)
    {
        perror("sigaction(SIGFPE)");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    char buf[8];

    install_handlers();

    printf("sigaction() demo: handlers for SIGSEGV, SIGINT, SIGFPE installed.\n");
    printf("Choose test:\n");
    printf("  a) SIGSEGV (dereference NULL)\n");
    printf("  b) SIGINT  (press Ctrl-C while sleeping)\n");
    printf("  c) SIGFPE  (integer divide by zero)\n");
    printf("  q) quit\n");
    printf("Enter choice: ");
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin))
        return 0;

    switch (buf[0])
    {
    case 'a':
    case 'A':
        trigger_sigsegv();
        break;
    case 'b':
    case 'B':
        printf("Sleeping for 10 seconds. Press Ctrl-C to send SIGINT.\n");
        fflush(stdout);
        sleep(10);
        printf("Sleep ended.\n");
        break;
    case 'c':
    case 'C':
        trigger_sigfpe();
        break;
    case 'q':
    case 'Q':
    default:
        printf("Exiting program.\n");
        break;
    }

    printf("Program finishing normally.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P10$ gcc 10.c -o 10

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P10$ ./10
sigaction() demo: handlers for SIGSEGV, SIGINT, SIGFPE installed.
Choose test:
  a) SIGSEGV (dereference NULL)
  b) SIGINT  (press Ctrl-C while sleeping)
  c) SIGFPE  (integer divide by zero)
  q) quit

------------------------------------------------------------------
Enter choice: a
Triggering SIGSEGV by writing to NULL pointer...

*** Caught signal 11: Segmentation fault
  siginfo: si_signo=11, si_errno=0, si_code=1
  SIGSEGV faulting address: (nil)
Exiting due to fatal signal 11.
------------------------------------------------------------------
Enter choice: b
Sleeping for 10 seconds. Press Ctrl-C to send SIGINT.
^C
*** Caught signal 2: Interrupt
  siginfo: si_signo=2, si_errno=0, si_code=128
Sleep ended.
Program finishing normally.
------------------------------------------------------------------
Enter choice: c
Triggering SIGFPE by dividing by zero...

*** Caught signal 8: Floating point exception
  siginfo: si_signo=8, si_errno=0, si_code=1
  SIGFPE: trap code (si_code) = 1
Exiting due to fatal signal 8.
------------------------------------------------------------------
Enter choice: q
Exiting program.
Program finishing normally.
============================================================================
*/
