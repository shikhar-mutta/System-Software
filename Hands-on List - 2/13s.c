/*
============================================================================
Name : 13s.c
Author : Shikhar Mutta
Description : Write two programs: first program is waiting to catch SIGSTOP signal,
              the second program will send the signal (using kill system call).
              Find out whether the first program is able to catch the signal or not.
Date: 18th September 2025.
============================================================================
*/

// Sender
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid = (pid_t)atoi(argv[1]);

    printf("sender: sending SIGSTOP to %d\n", pid);
    if (kill(pid, SIGSTOP) == -1)
    {
        fprintf(stderr, "sender: kill(SIGSTOP) failed: %s\n", strerror(errno));
        return 1;
    }
    printf("sender: SIGSTOP sent. Process %d should now be stopped.\n", pid);
    fflush(stdout);

    sleep(2);

    printf("sender: sending SIGCONT to %d\n", pid);
    if (kill(pid, SIGCONT) == -1)
    {
        fprintf(stderr, "sender: kill(SIGCONT) failed: %s\n", strerror(errno));
        return 1;
    }
    printf("sender: SIGCONT sent. Process %d should resume (and run SIGCONT handler).\n", pid);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P13$ gcc 13c.c -o 13c

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P13$ ./13s 22175
sender: sending SIGSTOP to 22175
sender: SIGSTOP sent. Process 22175 should now be stopped.
sender: sending SIGCONT to 22175
sender: SIGCONT sent. Process 22175 should resume (and run SIGCONT handler).

============================================================================
*/
