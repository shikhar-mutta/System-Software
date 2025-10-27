/*
============================================================================
Name : 13c.c
Author : Shikhar Mutta
Description : Write two programs: first program is waiting to catch SIGSTOP signal,
              the second program will send the signal (using kill system call).
              Find out whether the first program is able to catch the signal or not.
Date: 18th September 2025.
============================================================================
*/

// Catcher
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static void cont_handler(int sig)
{
    (void)sig;
    printf("catcher: received SIGCONT (handler executed)\n");
    fflush(stdout);
}

int main(void)
{
    struct sigaction act;

    printf("catcher: PID = %d\n", getpid());
    fflush(stdout);

    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_DFL;
    if (sigaction(SIGSTOP, &act, NULL) == -1)
    {
        printf("catcher: sigaction(SIGSTOP) failed: %s\n", strerror(errno));
    }
    else
    {
        printf("catcher: sigaction(SIGSTOP) succeeded unexpectedly\n");
    }

    memset(&act, 0, sizeof(act));
    act.sa_handler = cont_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sigaction(SIGCONT, &act, NULL) == -1)
    {
        fprintf(stderr, "catcher: sigaction(SIGCONT) failed: %s\n", strerror(errno));
        return 1;
    }
    else
    {
        printf("catcher: sigaction(SIGCONT) installed\n");
    }

    printf("catcher: now waiting (use sender to send SIGSTOP then SIGCONT)...\n");
    fflush(stdout);

    while (1)
    {
        pause();
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P13$ gcc 13s.c -o 13s

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P13$ ./13c
catcher: PID = 22175
catcher: sigaction(SIGSTOP) failed: Invalid argument
catcher: sigaction(SIGCONT) installed
catcher: now waiting (use sender to send SIGSTOP then SIGCONT)...

[1]+  Stopped                 ./13c
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P13$ catcher: received SIGCONT (handler executed)

============================================================================
*/
