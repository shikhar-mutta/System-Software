/*
============================================================================
Name : 11.c
Author : Shikhar Mutta
Description : Write a program to open a file, duplicate the file descriptor and append the file with both the
descriptors and check whether the file is updated properly or not.
            a. use dup
            b. use dup2
            c. use fcntl
Date: 22th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd, fd_dup1, fd_dup2, fd_dup3;

    fd = open("dup_test.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }

    fd_dup1 = dup(fd);
    if (fd_dup1 == -1)
    {
        perror("dup");
        exit(1);
    }

    fd_dup2 = dup2(fd, 10);
    if (fd_dup2 == -1)
    {
        perror("dup2");
        exit(1);
    }

    fd_dup3 = fcntl(fd, F_DUPFD, 20);
    if (fd_dup3 == -1)
    {
        perror("fcntl");
        exit(1);
    }

    write(fd, "Original FD\n", 12);
    write(fd_dup1, "dup() FD\n", 9);
    write(fd_dup2, "dup2() FD\n", 10);
    write(fd_dup3, "fcntl FD\n", 9);

    printf("Wrote to dup_test.txt using 4 descriptors (fd=%d, dup=%d, dup2=%d, fcntl=%d)\n",
           fd, fd_dup1, fd_dup2, fd_dup3);

    close(fd);
    close(fd_dup1);
    close(fd_dup2);
    close(fd_dup3);

    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P11$ gcc 11.c -o 11

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P11$ ./11

Wrote to dup_test.txt using 4 descriptors (fd=3, dup=4, dup2=10, fcntl=20)

-> check the file :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P11$ cat dup_test.txt
Original FD
dup() FD
dup2() FD
fcntl FD

============================================================================
*/


