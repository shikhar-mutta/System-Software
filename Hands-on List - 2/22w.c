/*
============================================================================
Name : 22.c
Author : Shikhar Mutta
Description : Write a program to wait for data to be written into FIFO within 10 seconds, use select system call with FIFO.
Date: 19th Sep, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h> 

#define FIFO_NAME "myfifo"

int main(int argc, char *argv[])
{
    int fd;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fd, argv[1], strlen(argv[1])) == -1)
    {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Message sent to FIFO: %s\n", argv[1]);
    close(fd);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P22$ gcc 22w.c -o 22w

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P22$ ./22w Shikhar
Message sent to FIFO: Shikhar

============================================================================
*/
