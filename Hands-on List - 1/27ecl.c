/*
============================================================================
Name : 27ecl.c
Author : Shikhar Mutta
Description : Write a program to execute ls -Rl by the following system calls
                    a. execl
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("Executing ls -Rl using execl\n");
    execl("/bin/ls", "ls", "-Rl", NULL);
    perror("execl failed");
    return 1;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P27$ gcc 27ecl.c -o 27ecl

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P27$ ./27ecl
Executing ls -Rl using execl
.:
total 104
-rwxrwxr-x 1 shikhar shikhar 16104 Aug 20 13:51 execle_ls
-rw-rw-r-- 1 shikhar shikhar   296 Aug 20 13:48 execle_ls.c

============================================================================
*/
