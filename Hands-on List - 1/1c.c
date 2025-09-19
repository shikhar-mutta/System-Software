/*
============================================================================
Name : 1c.c
Author : Shikhar Mutta
Description : Create the following types of a files using (i) shell command (ii) system call
                        c. FIFO (mkfifo Library Function or mknod system call)
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main(void)
{
    const char *fifo = "myfifo";
    if (mkfifo(fifo, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            return 1;
        }
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
    {
        int fr = open(fifo, O_RDONLY);
        if (fr == -1)
        {
            perror("open fifo for read");
            return 1;
        }
        char buf[256];
        ssize_t n = read(fr, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = '\0';
            printf("Child read: %s\n", buf);
        }
        else
        {
            printf("Child read nothing or error\n");
        }
        close(fr);
        return 0;
    }
    else
    {
        sleep(1);
        int fw = open(fifo, O_WRONLY);
        if (fw == -1)
        {
            perror("open fifo for write");
            return 1;
        }
        const char *msg = "Message through FIFO\n";
        if (write(fw, msg, strlen(msg)) == -1)
        {
            perror("write fifo");
            close(fw);
            return 1;
        }
        close(fw);
        wait(NULL);
        return 0;
    }
}

/*
============================================================================
Compilation :

[1] FIFO using shell command :
1. Create a FIFO using mkfifo:
    shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ mkfifo myfifo
    # or explicitly set permissions: mkfifo -m 0666 myfifo

2. Verify:
    shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ ls -l myfifo
    # output shows a 'p' at the start, eg: prw-r--r-- 1 you you 0 ... myfifo
    prw-rw-r-- 1 shikhar shikhar 0 Aug 20 03:33 myfifo


3. Use the FIFO (requires one process to read and another to write). Open two terminals (A and B):

    Terminal A (reader):
    shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ cat myfifo

    Terminal B (writer):
    shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ echo "Hello through FIFO" > myfifo

4. Terminal A (reader):
    hikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ cat myfifo
    Hello through FIFO


[2] FIFO using system call :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ gcc 1c.c -o 1c
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$./1c
Child read: Hello through FIFO

=> You can also use background processes in a single terminal:
Terminal A (reader):
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ cat myfifo &
[2] 34520
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ Hello through FIFO

Terminal B (writer):
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ echo "Hello through FIFO" > myfifo

============================================================================
*/
