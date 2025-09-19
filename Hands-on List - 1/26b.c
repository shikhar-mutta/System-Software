/*
============================================================================
Name : 26b.c
Author : Shikhar Mutta
Description : Write a program to execute an executable program.
        b. pass some input to an executable program. (for example execute an executable of $./a.out name )
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>

int main(int argc, char *argv[]) {
    if(argc > 1) {
        printf("Hello, %s! This is program a.\n", argv[1]);
    } else {
        printf("Hello! This is program a.\n");
    }
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P26$ gcc 26b.c -o 26b.out

============================================================================
*/


