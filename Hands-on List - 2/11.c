/*
============================================================================
Name : 11.c
Author : Shikhar Mutta
Description : Write a program to ignore a SIGINT signal then reset the default action of the SIGINT signal use sigaction system call.
Date: 18th September 2025.
============================================================================
*/

#define _POSIX_C_SOURCE 200809L   

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction - ignore");
        exit(EXIT_FAILURE);
    }
    printf("SIGINT is now being ignored. Press Ctrl-C (will be ignored).\n");
    sleep(5);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction - default");
        exit(EXIT_FAILURE);
    }
    printf("SIGINT reset to default. Press Ctrl-C again to terminate.\n");

    while (1) {
        printf("Running... press Ctrl-C to exit now.\n");
        sleep(2);
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P11$ gcc 11.c -o 11

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P11$ ./11
SIGINT is now being ignored. Press Ctrl-C (will be ignored).
^C SIGINT reset to default. Press Ctrl-C again to terminate.
Running... press Ctrl-C to exit now.
Running... press Ctrl-C to exit now.
^C

============================================================================
*/
