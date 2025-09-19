/*
============================================================================
Name : 12.c
Author : Shikhar Mutta
Description : Write a program to find out the opening mode of a file. Use fcntl.
Date: 22th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd, flags, accmode;
    fd = open("mode_test.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        perror("fcntl");
        exit(1);
    }
    accmode = flags & O_ACCMODE;
    printf("File opened with mode: ");
    if (accmode == O_RDONLY)
        printf("Read only\n");
    else if (accmode == O_WRONLY)
        printf("Write only\n");
    else if (accmode == O_RDWR)
        printf("Read/Write\n");
    if (flags & O_APPEND)
        printf("O_APPEND flag is set\n");
    if (flags & O_NONBLOCK)
        printf("O_NONBLOCK flag is set\n");
    if (flags & O_SYNC)
        printf("O_SYNC flag is set\n");
    if (flags & O_CREAT)
        printf("O_CREAT flag is set (only relevant at open time)\n");
    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P12$ gcc 12.c -o 12

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P12$ ./12
File opened with mode: Read/Write
O_APPEND flag is set

============================================================================
*/
