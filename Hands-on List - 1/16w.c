/*
============================================================================
Name : 16w.c
Author : Shikhar Mutta
Description : 16. Write a program to perform mandatory locking.
                    a. Implement write lock
                    b. Implement read lock
Date: 23th Aug, 2025.
============================================================================
*/
#include<stdio.h>  
#include<stdlib.h>  
#include<unistd.h>  
#include<fcntl.h>  

int main() {
    struct flock lock;
    int fd;

    fd = open("16lockfile.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    
    lock.l_type = F_WRLCK;   
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;          

    printf("Trying to acquire write lock...\n");
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("fcntl");
        close(fd);
        exit(1);
    }
    printf("Write lock acquired. Press Enter to release...\n");
    getchar();

    lock.l_type = F_UNLCK;   
    fcntl(fd, F_SETLK, &lock);

    close(fd);
    return 0;
}


/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P16$ gcc 16w.c -o 16w

-> Run ./16w in one terminal (acquires write lock):
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P16$ ./16w
Trying to acquire write lock...
Write lock acquired. Press Enter to release...

============================================================================
*/


