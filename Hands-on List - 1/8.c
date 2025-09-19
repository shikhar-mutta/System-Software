/*
============================================================================
Name : 8.c
Author : Shikhar Mutta
Description : Write a program to open a file in read only mode, read line by line and display each line as it is read. Close the file when end of file is reached.
Date : 21th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 256

int main(int argc, char *argv[])
{
    int fd;
    char buffer[SIZE];
    ssize_t n;
    char line[SIZE];
    int idx = 0;
    if (argc != 2)
    {
        write(2, "Usage: ./8 <filename>\n", 29);
        exit(1);
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    while ((n = read(fd, buffer, SIZE)) > 0)
    {
        for (int i = 0; i < n; i++)
        {
            line[idx++] = buffer[i];
            if (buffer[i] == '\n')
            {
                write(1, line, idx);
                idx = 0;
            }
        }
    }
    if (idx > 0)
    {
        write(1, line, idx);
    }
    if (n < 0)
    {
        perror("read");
    }
    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P8$ gcc 8.c -o 8

Create a test file:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P8$ echo -e "Line 1\nLine 2\nLine 3" > sample.txt

Run:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P8$ ./8 sample.txt
Line 1
Line 2
Line 3
============================================================================
*/
