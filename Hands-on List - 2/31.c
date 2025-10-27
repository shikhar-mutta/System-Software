/*
============================================================================
Name : 31.c
Author : Shikhar Mutta
Description : Write a program to create a semaphore and initialize value to the semaphore.
                    a. create a binary semaphore
                    b. create a counting semaphore
Date: 20th September 2025.
============================================================================
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define PATH "/tmp"
#define PROJ_BIN 11
#define PROJ_CNT 22

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main()
{
    key_t key_bin, key_cnt;
    int semid_bin, semid_cnt;
    union semun arg;

    key_bin = ftok(PATH, PROJ_BIN);
    key_cnt = ftok(PATH, PROJ_CNT);
    if (key_bin == -1 || key_cnt == -1)
    {
        perror("ftok");
        exit(1);
    }

    semid_bin = semget(key_bin, 1, IPC_CREAT | 0666);
    if (semid_bin == -1)
    {
        perror("semget (binary)");
        exit(1);
    }

    arg.val = 1;
    if (semctl(semid_bin, 0, SETVAL, arg) == -1)
    {
        perror("semctl (SETVAL binary)");
        exit(1);
    }

    printf("Binary semaphore created: semid=%d, initial value=1\n", semid_bin);

    semid_cnt = semget(key_cnt, 1, IPC_CREAT | 0666);
    if (semid_cnt == -1)
    {
        perror("semget (counting)");
        exit(1);
    }

    arg.val = 3;
    if (semctl(semid_cnt, 0, SETVAL, arg) == -1)
    {
        perror("semctl (SETVAL counting)");
        exit(1);
    }

    printf("Counting semaphore created: semid=%d, initial value=3\n", semid_cnt);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P31$ gcc 31.c -o 31

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P31$ ./31
Binary semaphore created: semid=0, initial value=1
Counting semaphore created: semid=1, initial value=3

============================================================================
*/
