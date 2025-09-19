/*
============================================================================
Name : 4.c
Author : Shikhar Mutta
Description : Write a program to open an existing file with read write mode. Try O_EXCL flag also.
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd;
    fd = open("myfile.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    printf("File opened successfully. File descriptor: %d\n", fd);
    close(fd);
    fd = open("myfile.txt", O_RDWR | O_CREAT | O_EXCL, 0644); 

    if (fd == -1)
    {
        perror("open with O_EXCL");
    }
    else
    {
        printf("File created (unexpected, since it already exists). FD: %d\n", fd);
        close(fd);
    }
    return 0;
}

/*
============================================================================
Compilation :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P4$ gcc 4.c -o 4
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P4$ ./4
File opened successfully. File descriptor: 3
open with O_EXCL: File exists

============================================================================
*/
