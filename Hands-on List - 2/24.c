/*
============================================================================
Name : 24.c
Author : Shikhar Mutta
Description : Write a program to create a message queue and print the key and message queue id.
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(void) {
    key_t key;
    int msqid;

    /* Generate a unique key using ftok */
    key = ftok(".", 'A');   // '.' = current dir, 'A' = project id
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    /* Create a new message queue, rw perms for user */
    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Message Queue created successfully!\n");
    printf("Key = %d\n", key);
    printf("Message Queue ID = %d\n", msqid);

    return 0;
}


/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P24$ gcc -o 24 24.c

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P24$ ./24
Message Queue created successfully!
Key = 1090860320
Message Queue ID = 0

============================================================================
*/
