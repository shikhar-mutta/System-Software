/*
============================================================================
Name : 20w.c
Author : Shikhar Mutta
Description : Write two programs so that both can communicate by FIFO -Use one way communication.
Date: 19th September 2025.
============================================================================
*/

// Writer
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *fifo = (argc > 1) ? argv[1] : "./myfifo";
    char buf[4096];

    if (mkfifo(fifo, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return EXIT_FAILURE;
        }
    }

    int fd = open(fifo, O_WRONLY);
    if (fd == -1)
    {
        perror("open fifo for write");
        return EXIT_FAILURE;
    }

    printf("Writer: opened %s for writing. Type lines and press Enter. Ctrl-D to exit.\n", fifo);
    while (fgets(buf, sizeof(buf), stdin))
    {
        ssize_t len = strlen(buf);
        ssize_t w = write(fd, buf, len);
        if (w == -1)
        {
            perror("write");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    close(fd);
    printf("Writer: EOF on stdin, exiting.\n");
    return EXIT_SUCCESS;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P20$ gcc 20w.c -o 20w

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P20$ ./20w
Writer: opened ./myfifo for writing. Type lines and press Enter. Ctrl-D to exit.
Hello World!
This test perform by SM.
Writer: EOF on stdin, exiting.

============================================================================
*/
