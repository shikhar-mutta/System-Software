/*
============================================================================
Name : 18a.c
Author : Shikhar Mutta
Description : Write a program to perform Record locking.
                    a. Implement write lock
Create three records in a file. Whenever you access a particular record, first lock it then modify/access to avoid race condition.
Date: 23th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define RECORDS 3

int lock_record(int fd, int recno, short lock_type)
{
    struct flock lock;
    lock.l_type = lock_type;
    lock.l_whence = SEEK_SET;
    lock.l_start = recno * sizeof(int);
    lock.l_len = sizeof(int);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror("fcntl lock");
        return -1;
    }
    return 0;
}

int unlock_record(int fd, int recno)
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = recno * sizeof(int);
    lock.l_len = sizeof(int);
    lock.l_pid = getpid();
    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        perror("fcntl unlock");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <record_no 0-2> <r|w>\n", argv[0]);
        exit(1);
    }
    int recno = atoi(argv[1]);
    char mode = argv[2][0];
    if (recno < 0 || recno >= RECORDS)
    {
        fprintf(stderr, "Invalid record number. Use 0,1,2.\n");
        exit(1);
    }
    int fd = open("records.dat", O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    off_t size = lseek(fd, 0, SEEK_END);
    if (size < RECORDS * sizeof(int))
    {
        int init[RECORDS] = {100, 200, 300};
        write(fd, init, sizeof(init));
    }
    if (mode == 'r')
    {
        printf("Trying to acquire read lock on record %d...\n", recno);
        lock_record(fd, recno, F_RDLCK);
        printf("Read lock acquired on record %d\n", recno);

        int value;
        lseek(fd, recno * sizeof(int), SEEK_SET);
        read(fd, &value, sizeof(int));
        printf("Read record %d: %d\n", recno, value);

        sleep(5);

        unlock_record(fd, recno);
        printf("Read lock released on record %d\n", recno);
    }
    else if (mode == 'w')
    {
        printf("Trying to acquire write lock on record %d...\n", recno);
        lock_record(fd, recno, F_WRLCK);
        printf("Write lock acquired on record %d\n", recno);
        int value;
        lseek(fd, recno * sizeof(int), SEEK_SET);
        read(fd, &value, sizeof(int));
        printf("Current value at record %d: %d\n", recno, value);
        value += 10;
        lseek(fd, recno * sizeof(int), SEEK_SET);
        write(fd, &value, sizeof(int));
        printf("Updated record %d to: %d\n", recno, value);
        sleep(5);
        unlock_record(fd, recno);
        printf("Write lock released on record %d\n", recno);
    }
    else
    {
        fprintf(stderr, "Invalid mode. Use r for read, w for write.\n");
    }

    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P18$ gcc 18.c -o 18

Initialize & test:

=> ./18 1 w   # Try write lock on same record (will wait until released) :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P18$ ./18 1 w
Trying to acquire write lock on record 1...
Write lock acquired on record 1
Current value at record 1: 200
Updated record 1 to: 210
Write lock released on record 1

=> ./18 2 w   # Can modify record 2 independently

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P18$ ./18 2 w
Trying to acquire write lock on record 2...
Write lock acquired on record 2
Current value at record 2: 300
Updated record 2 to: 310
Write lock released on record 2

============================================================================
*/
