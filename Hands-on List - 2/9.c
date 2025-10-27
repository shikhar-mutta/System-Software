/*
============================================================================
Name : 9.c
Author : Shikhar Mutta
Description : Write a program to ignore a SIGINT signal then reset the default action of the SIGINT signal - Use signal system call.
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    signal(SIGINT, SIG_IGN);
    printf("SIGINT is now being ignored. Try pressing Ctrl-C (nothing will happen).\n");
    sleep(5);

    signal(SIGINT, SIG_DFL);
    printf("SIGINT has been reset to default. Press Ctrl-C again to terminate.\n");

    while (1) {
        printf("Running... press Ctrl-C now to exit.\n");
        sleep(2);
    }
    return 0;
}


/*
Compilation :
============================================================================

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P9$ gcc 9.c -o 9

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P9$ ./9
SIGINT is now being ignored. Try pressing Ctrl-C (nothing will happen).
^C SIGINT has been reset to default. Press Ctrl-C again to terminate.
Running... press Ctrl-C now to exit.
Running... press Ctrl-C now to exit.
^C

============================================================================
*/
