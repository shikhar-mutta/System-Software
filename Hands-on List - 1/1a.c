/*
============================================================================
Name : 1a.c
Author : Shikhar Mutta
Description : Create the following types of a files using (i) shell command (ii) system call
                        a. soft link (symlink system call)
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>x
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>

int main(void)
{
    const char *orig = "1aoriginal.txt";
    const char *slink = "hp1as.txt";
    int fd = open(orig, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open original");
        return 1;
    }
    const char *msg = "Hello from original (symlink demo)\n";
    if (write(fd, msg, strlen(msg)) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);

    if (symlink(orig, slink) == -1)
    {
        if (errno != EEXIST)
        {
            perror("symlink");
            return 1;
        }
    }

    struct stat st_link, st_orig;

    if (lstat(slink, &st_link) == -1)
    {
        perror("lstat symlink");
        return 1;
    }

    if (stat(orig, &st_orig) == -1)
    {
        perror("stat original");
        return 1;
    }

    printf("lstat('%s') says it's a symlink (mode=%o, inode=%ld)\n",
           slink, st_link.st_mode & 07777, (long)st_link.st_ino);
    printf("stat('%s') -> inode=%ld, nlink=%ld\n",
           orig, (long)st_orig.st_ino, (long)st_orig.st_nlink);

    char buf[PATH_MAX];

    ssize_t r = readlink(slink, buf, sizeof(buf) - 1);
    if (r == -1)
    {
        perror("readlink");
        return 1;
    }
    buf[r] = '\0';
    printf("'%s' -> '%s'\n", slink, buf);

    int fr = open(slink, O_RDONLY);
    if (fr == -1)
    {
        perror("open symlink for reading");
        return 1;
    }
    char data[256];
    ssize_t n = read(fr, data, sizeof(data) - 1);
    if (n >= 0)
    {
        data[n] = '\0';
        printf("Content via symlink:\n%s", data);
    }
    close(fr);

    return 0;
}

/*
============================================================================
Compilation :

[1] SoftLink shell command :
1. Create Original File :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ echo "Hello — original file (symlink demo)" > original.txt

2. Create a symbolic (soft) link:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ ln -s original.txt hp1ac_symlink.txt

3. Validate:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ ls -l original.txt hp1ac_symlink.txt
lrwxrwxrwx 1 shikhar shikhar 12 Aug 20 02:43 hp1ac_symlink.txt -> original.txt
-rw-rw-r-- 1 shikhar shikhar 40 Aug 20 02:46 original.txt

4. Read through softlink:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ cat hp1ac_symlink.txt
Hello — original file (symlink demo)



[2] SoftLink using system call :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P1$ gcc 1a.c -o 1a
./1a
lstat('hp1as.txt') says it's a symlink (mode=777, inode=2112038)
stat('original.txt') -> inode=2111908, nlink=1
'hp1as.txt' -> 'original.txt'
Content via symlink:
Hello from original (symlink demo)

============================================================================
*/