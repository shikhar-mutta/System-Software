/*
============================================================================
Name : 16r.c
Author : Shikhar Mutta
Description : 16. Write a program to perform mandatory locking.
                    a. Implement write lock
                    b. Implement read lock
Date: 23th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    struct flock lock;
    int fd;
    fd = open("16lockfile.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    printf("Trying to acquire read lock...\n");
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        close(fd);
        exit(1);
    }
    printf("Read lock acquired. Press Enter to release...\n");
    getchar();

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);
    return 0;
}


/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P16$ gcc 16r.c -o 16r

->Run ./16r in another terminal:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P16$ ./16r
Trying to acquire read lock...
Read lock acquired. Press Enter to release...

============================================================================
*/



