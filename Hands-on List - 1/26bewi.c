/*
============================================================================
Name : 26ewi.c
Author : Shikhar Mutta
Description : Write a program to execute an executable program.
        b. pass some input to an executable program. (for example execute an executable of $./a.out name )
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char *program = "./a.out";
    char *arg1 = "Shikhar";
    char *argv[] = {program, arg1, NULL};
    printf("Parent process PID = %d\n", getpid());
    printf("Executing program %s with argument: %s\n", program, arg1);
    if (execv(program, argv) < 0)
    {
        perror("execv failed");
        return 1;
    }
    printf("This will not be printed.\n");
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P26$ gcc 26bewl.c -o 26bewl

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P26$ ./26bewl
Parent process PID = 13235
Executing program ./26b.out with argument: Shikhar
Hello! This is program a.
============================================================================
*/

