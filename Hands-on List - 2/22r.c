/*
============================================================================
Name : 22r.c
Author : Shikhar Mutta
Description : Write a program to wait for data to be written into FIFO within 10 seconds, use select system call with FIFO.
Date: 19th Sep, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define FIFO_NAME "myfifo"
#define TIMEOUT_SECONDS 10

int main()
{
    int fd, ret;
    fd_set readfds;
    struct timeval tv;
    char buffer[256];

    if (access(FIFO_NAME, F_OK) == -1)
    {
        if (mkfifo(FIFO_NAME, 0666) == -1)
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        printf("FIFO created: %s\n", FIFO_NAME);
    }

    fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for data in FIFO '%s' (timeout: %d seconds)...\n",
           FIFO_NAME, TIMEOUT_SECONDS);

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    tv.tv_sec = TIMEOUT_SECONDS;
    tv.tv_usec = 0;

    ret = select(fd + 1, &readfds, NULL, NULL, &tv);

    if (ret == -1)
    {
        perror("select");
        close(fd);
        exit(EXIT_FAILURE);
    }
    else if (ret == 0)
    {
        printf("Timeout: No data received within %d seconds.\n", TIMEOUT_SECONDS);
        close(fd);
        exit(EXIT_FAILURE);
    }
    else
    {

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1)
        {
            perror("read");
            close(fd);
            exit(EXIT_FAILURE);
        }

        buffer[bytes_read] = '\0';
        printf("Received %zd bytes: %s\n", bytes_read, buffer);
    }

    close(fd);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P22$ gcc 22r.c -o 22r

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P22$ ./22r
Waiting for data in FIFO 'myfifo' (timeout: 10 seconds)...
Received 7 bytes: Shikhar

============================================================================
*/
