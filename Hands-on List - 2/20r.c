/*
============================================================================
Name : 20r.c
Author : Shikhar Mutta
Description : Write two programs so that both can communicate by FIFO -Use one way communication.
Date: 19th September 2025.
============================================================================
*/

// Reader
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *fifo = (argc > 1) ? argv[1] : "./myfifo";
    char buf[4096];

    int fd = open(fifo, O_RDONLY);
    if (fd == -1) {
        perror("open fifo for read");
        return EXIT_FAILURE;
    }

    printf("Reader: opened %s for reading. Waiting for data...\n", fifo);
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        ssize_t written = 0;
        while (written < r) {
            ssize_t w = write(STDOUT_FILENO, buf + written, r - written);
            if (w == -1) { perror("write to stdout"); close(fd); return EXIT_FAILURE; }
            written += w;
        }
    }

    if (r == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    printf("\nReader: writer closed FIFO, exiting.\n");
    return EXIT_SUCCESS;
}


/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P20$ gcc 20r.c -o 20r

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P20$ ./20r
Reader: opened ./myfifo for reading. Waiting for data...
Hello World!
This test perform by SM.

Reader: writer closed FIFO, exiting.

============================================================================
*/
