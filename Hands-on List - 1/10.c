/*
============================================================================
Name : 10.c
Author : Shikhar Mutta
Description : Write a program to open a file with read write mode, write 10 bytes, move the file pointer by 10
bytes (use lseek) and write again 10 bytes.
        a. check the return value of lseek
        b. open the file with od and check the empty spaces in between the data.
Date: 21th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int fd;
    off_t offset;
    char buf1[10] = "ABCDEFGHIJ";
    char buf2[10] = "1234567890";

    fd = open("holefile.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    if (write(fd, buf1, 10) != 10)
    {
        perror("write buf1");
        close(fd);
        exit(1);
    }
    offset = lseek(fd, 10, SEEK_CUR);
    if (offset == -1)
    {
        perror("lseek");
        close(fd);
        exit(1);
    }
    printf("lseek return value (new offset): %ld\n", (long)offset);
    if (write(fd, buf2, 10) != 10)
    {
        perror("write buf2");
        close(fd);
        exit(1);
    }

    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
[1]  check the return value of lseek :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P10$ gcc 10.c -o 10
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P10$ ./10
lseek return value (new offset): 20

This means:
First write took bytes 0–9
lseek skipped bytes 10–19
Second write filled bytes 20–29

[2] open the file with od and check the empty spaces in between the data :
Use octal/hex dump:

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P10$ od -c holefile.txt
Expected output:
0000000   A   B   C   D   E   F   G   H   I   J  \0  \0  \0  \0  \0  \0
0000020  \0  \0  \0  \0   1   2   3   4   5   6   7   8   9   0
0000036

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P10$ od -c holefile.txt
0000000   A   B   C   D   E   F   G   H   I   J  \0  \0  \0  \0  \0  \0
0000020  \0  \0  \0  \0   1   2   3   4   5   6   7   8   9   0
0000036

============================================================================
*/
