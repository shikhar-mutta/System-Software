/*
============================================================================
Name : 16.c
Author : Shikhar Mutta
Description : Write a program to send and receive data from parent to child vice versa. Use two way communication.
Date: 19th Sep, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

static ssize_t write_all(int fd, const void *buf, size_t count)
{
    const char *p = buf;
    size_t left = count;
    while (left > 0)
    {
        ssize_t w = write(fd, p, left);
        if (w <= 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        left -= w;
        p += w;
    }
    return (ssize_t)count;
}

static ssize_t read_all(int fd, void *buf, size_t count)
{
    char *p = buf;
    size_t left = count;
    while (left > 0)
    {
        ssize_t r = read(fd, p, left);
        if (r < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (r == 0)
            break;
        left -= r;
        p += r;
    }
    return (ssize_t)(count - left);
}

int main(void)
{
    int p2c[2];
    int c2p[2];

    if (pipe(p2c) == -1)
    {
        perror("pipe p2c");
        exit(EXIT_FAILURE);
    }
    if (pipe(c2p) == -1)
    {
        perror("pipe c2p");
        close(p2c[0]);
        close(p2c[1]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        close(p2c[1]);
        close(c2p[0]);

        char buf[256];
        ssize_t n = read_all(p2c[0], buf, sizeof(buf));
        if (n < 0)
        {
            perror("child read");
            close(p2c[0]);
            close(c2p[1]);
            _exit(2);
        }
        if ((size_t)n >= sizeof(buf))
            n = sizeof(buf) - 1;
        buf[n] = '\0';

        printf("Child: received from parent: %s\n", buf);

        const char *reply = "Hello Parent — message received loud and clear!";
        if (write_all(c2p[1], reply, strlen(reply)) == -1)
        {
            perror("child write");
        }

        close(p2c[0]);
        close(c2p[1]);
        _exit(0);
    }
    else
    {
        close(p2c[0]);
        close(c2p[1]);

        const char *msg = "Hello Child, this is Parent!";
        if (write_all(p2c[1], msg, strlen(msg)) == -1)
        {
            perror("parent write");
            close(p2c[1]);
            close(c2p[0]);
            waitpid(pid, NULL, 0);
            exit(EXIT_FAILURE);
        }

        close(p2c[1]);

        char reply_buf[256];
        ssize_t rn = read_all(c2p[0], reply_buf, sizeof(reply_buf));
        if (rn < 0)
        {
            perror("parent read");
        }
        else
        {
            if ((size_t)rn >= sizeof(reply_buf))
                rn = sizeof(reply_buf) - 1;
            reply_buf[rn] = '\0';
            printf("Parent: received from child: %s\n", reply_buf);
        }

        close(c2p[0]);
        waitpid(pid, NULL, 0);
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P16$ gcc 16.c -o 16

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P16$ ./16
Child: received from parent: Hello Child, this is Parent!
Parent: received from child: Hello Parent — message received loud and clear!

============================================================================
*/
