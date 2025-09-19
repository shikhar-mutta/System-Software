/*
============================================================================
Name : 17rtc.c
Author : Shikhar Mutta
Description : Write a program to simulate online ticket reservation.Implement write lock Write a program to open a file, store a ticket number and exit. Write a separate program, to open the file, implement write lock, read the ticket number, increment the number and print the new ticket number then close the file.
Date: 23th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd;
    int ticket;
    struct flock lock;

    fd = open("17ticket.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    printf("Trying to acquire write lock...\n");
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl");
        close(fd);
        exit(1);
    }
    printf("Write lock acquired!\n");
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &ticket, sizeof(ticket)) == -1)
    {
        perror("read");
        close(fd);
        exit(1);
    }
    printf("Current ticket number: %d\n", ticket);
    ticket++;
    lseek(fd, 0, SEEK_SET);
    if (write(fd, &ticket, sizeof(ticket)) == -1)
    {
        perror("write");
        close(fd);
        exit(1);
    }
    printf("New ticket reserved! Ticket number = %d\n", ticket);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    return 0;
}


/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P17$ gcc 17rtc.c -o 17rtc


-> Run multiple reservations:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P17$ ./17rtc
./17rtc
./17rtc

Trying to acquire write lock...
Write lock acquired!
Current ticket number: 0
New ticket reserved! Ticket number = 1
Trying to acquire write lock...
Write lock acquired!
Current ticket number: 1
New ticket reserved! Ticket number = 2
Trying to acquire write lock...
Write lock acquired!
Current ticket number: 2
New ticket reserved! Ticket number = 3

============================================================================
*/


