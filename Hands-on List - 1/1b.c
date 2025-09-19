/*
============================================================================
Name : 1b.c
Author : Shikhar Mutta
Description : Create the following types of a files using (i) shell command (ii) system call
                        b. hard link (link system call)
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int main(void)
{
    const char *orig = "1boriginal_hard.txt";
    const char *hlink = "hp1bs.txt";
    int fd = open(orig, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open original");
        return 1;
    }
    const char *msg = "Hello from original (hardlink demo)\n";
    if (write(fd, msg, strlen(msg)) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);
    if (link(orig, hlink) == -1)
    {
        perror("link");
        return 1;
    }
    struct stat st1, st2;
    if (stat(orig, &st1) == -1)
    {
        perror("stat orig");
        return 1;
    }
    if (stat(hlink, &st2) == -1)
    {
        perror("stat hlink");
        return 1;
    }
    printf("orig: inode=%ld, nlink=%ld\n", (long)st1.st_ino, (long)st1.st_nlink);
    printf("hlink: inode=%ld, nlink=%ld\n", (long)st2.st_ino, (long)st2.st_nlink);
    if (unlink(orig) == -1)
        perror("unlink orig");
    else
        printf("Unlinked original name '%s'\n", orig);

    int fr = open(hlink, O_RDONLY);
    if (fr == -1)
    {
        perror("open hlink");
        return 1;
    }
    char buf[256];
    ssize_t n = read(fr, buf, sizeof(buf) - 1);
    if (n >= 0)
    {
        buf[n] = '\0';
        printf("Content via hardlink:\n%s", buf);
    }
    close(fr);

    return 0;
}

/*
============================================================================
Compilation :

[1] HardLink shell command :
1.Create an original file:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ echo "Hello — original file (hardlink demo)" > original_hard.txt

2.Create a hard link:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ ln original_hard.txt hp1bs.txt


[2] HardLink using system call :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ gcc 1b.c -o 1b
./1b
orig: inode=2112086, nlink=3
hlink: inode=2112086, nlink=3
Unlinked original name 'original_hard.txt'
Content via hardlink:
Hello from original (hardlink demo)

============================================================================
*/