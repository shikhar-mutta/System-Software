/*
============================================================================
Name : 14.c
Author : Shikhar Mutta
Description : Write a program to find the type of a file.
            a. Input should be taken from command line.
            b. program should be able to identify any type of a file.
Date: 22th Aug, 2025.
============================================================================
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/stat.h> 
#include <unistd.h> 

int main(int argc, char *argv[]) {
    struct stat st;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    if (lstat(argv[1], &st) == -1) {
        perror("lstat");
        exit(1);
    }
    printf("File: %s\n", argv[1]);

    if (S_ISREG(st.st_mode))
        printf("Type: Regular file\n");
    else if (S_ISDIR(st.st_mode))
        printf("Type: Directory\n");
    else if (S_ISCHR(st.st_mode))
        printf("Type: Character device\n");
    else if (S_ISBLK(st.st_mode))
        printf("Type: Block device\n");
    else if (S_ISFIFO(st.st_mode))
        printf("Type: FIFO/pipe\n");
    else if (S_ISLNK(st.st_mode))
        printf("Type: Symbolic link\n");
    else if (S_ISSOCK(st.st_mode))
        printf("Type: Socket\n");
    else
        printf("Type: Unknown\n");

    return 0;
}


/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P14$ gcc 14.c -o 14

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P14$./14 14sample.txt
File: 14sample.txt
Type: Regular file

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P14$./14 /etc
File: /etc
Type: Directory

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P14$./14 /dev/null
File: /dev/null
Type: Character device

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P14$./14 /tmp/mysock
File: /tmp/mysock
Type: Socket

============================================================================
*/


