/*
============================================================================
Name : 26ep.c
Author : Shikhar Mutta
Description : Write a program to execute an executable program.
        a. use some executable program
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h> 

int main()
{
    char *program = "./26a.out";      
    char *argv[] = {program, NULL}; 

    printf("Parent process PID = %d\n", getpid()); 
    printf("Executing program %s\n", program);
    
    if (execv(program, argv) < 0)
    {
        perror("execv failed");
        return 1;
    }
   
    printf("This line will not be printed.\n");
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P26$ gcc 26aep.c -o 26aep

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P26$ ./26aep
Parent process PID = 13112
Executing program ./26a.out
Hello! This is program a.

============================================================================
*/




