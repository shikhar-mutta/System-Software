/*
============================================================================
Name : 7.c
Author : Shikhar Mutta
Description : Write a program to copy file1 into file2 ($cp file1 file2).
Date: 21th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>     
#include <unistd.h>    
#include <stdlib.h>    

#define SIZE 1024

int main(int argc, char *argv[]) {
    int fd1, fd2;
    char buffer[SIZE];
    ssize_t n;
    if (argc != 3) {
        write(2, "Usage: ./mycp <source> <destination>\n", 38);
        exit(1);
    }
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        perror("open source");
        exit(1);
    }
    fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 == -1) {
        perror("open destination");
        close(fd1);
        exit(1);
    }
    while ((n = read(fd1, buffer, SIZE)) > 0) {
        if (write(fd2, buffer, n) != n) {
            perror("write");
            close(fd1);
            close(fd2);
            exit(1);
        }
    }
    if (n < 0) {
        perror("read");
    }
    close(fd1);
    close(fd2);
    return 0;
}

/*
============================================================================
Compilation :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P7$ gcc 7.c -o 7

1: Create a source file
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P7$ echo "Hello, Shikhar! This is file1." > file1.txt

2: Copy file1 → file2
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P7$ ./7 file1.txt file2.txt

3: Verify
hikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P7$ cat file2.txt

Output:
Hello, Shikhar! This is file1.
============================================================================
*/

