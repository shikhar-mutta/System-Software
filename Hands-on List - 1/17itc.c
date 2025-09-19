/*
============================================================================
Name : 17itc.c
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
    int ticket = 0;
    fd = open("17ticket.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    if (write(fd, &ticket, sizeof(ticket)) == -1)
    {
        perror("write");
        close(fd);
        exit(1);
    }
    printf("Ticket file initialized with ticket number = %d\n", ticket);
    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P17$ gcc 17itc.c -o 17itc

->Initialize ticket file:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P17$ ./17itc
Ticket file initialized with ticket number = 0

============================================================================
*/

