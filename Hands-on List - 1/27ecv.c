/*
============================================================================
Name : 27ecvp.c
Author : Shikhar Mutta
Description : Write a program to execute ls -Rl by the following system calls
                    a. execl
                    b. execlp
                    c. execle
                    d. execv
                    e. execvp
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
    char *argv[] = {"ls", "-Rl", NULL};

    printf("Executing ls -Rl using execv\n");
    execv("/bin/ls", argv);
    perror("execv failed");
    return 1;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P27$ gcc 27ecv.c -o 27ecv

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P27$ ./27ecv
Executing ls -Rl using execl
.:
total 104
-rwxrwxr-x 1 shikhar shikhar 16104 Aug 20 13:51 execle_ls
-rw-rw-r-- 1 shikhar shikhar   296 Aug 20 13:48 execle_ls.c

============================================================================
*/
