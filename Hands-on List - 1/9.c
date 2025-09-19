/*
============================================================================
Name : 9.c
Author : Shikhar Mutta
Description : Write a program to print the following information about a given file.
                            a. inode
                            b. number of hard links
                            c. uid
                            d. gid
                            e. size
                            f. block size
                            g. number of blocks
                            h. time of last access
                            i. time of last modification
                            j. time of last change
Date: 21th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    struct stat fileStat;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if (stat(argv[1], &fileStat) < 0)
    {
        perror("stat");
        exit(1);
    }

    printf("File: %s\n", argv[1]);
    printf("a. Inode number: %ld\n", (long)fileStat.st_ino);
    printf("b. Number of hard links: %ld\n", (long)fileStat.st_nlink);
    printf("c. UID: %d\n", fileStat.st_uid);
    printf("d. GID: %d\n", fileStat.st_gid);
    printf("e. Size: %ld bytes\n", (long)fileStat.st_size);
    printf("f. Block size: %ld bytes\n", (long)fileStat.st_blksize);
    printf("g. Number of blocks: %ld\n", (long)fileStat.st_blocks);
    printf("h. Time of last access: %s", ctime(&fileStat.st_atime));
    printf("i. Time of last modification: %s", ctime(&fileStat.st_mtime));
    printf("j. Time of last status change: %s", ctime(&fileStat.st_ctime));

    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P9$ gcc 9.c -o 9

Test with a file:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P9$ ./9 sample.txt

output:

File: sample.txt
a. Inode number: 2112190
b. Number of hard links: 1
c. UID: 1000
d. GID: 1000
e. Size: 31 bytes
f. Block size: 4096 bytes
g. Number of blocks: 8
h. Time of last access: Wed Aug 20 05:40:18 2025
i. Time of last modification: Wed Aug 20 05:40:18 2025
j. Time of last status change: Wed Aug 20 05:40:18 2025

============================================================================
*/

