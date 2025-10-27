/*
============================================================================
Name : 27r.c
Author : Shikhar Mutta
Description : Write a program to receive messages from the message queue.
                    a. with 0 as a flag
                    b. with IPC_NOWAIT as a flag
Date: 20th September 2025.
============================================================================
*/

// Reciver Program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>

#define MAX_MSG_SIZE 1024
#define MSG_KEY 1234

struct message
{
    long mtype;
    char mtext[MAX_MSG_SIZE];
};

int main()
{
    int msgid, choice;
    struct message msg;

    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("Message queue created/accessed with ID: %d\n", msgid);

    while (1)
    {
        printf("\n=== Message Receiver Options ===\n");
        printf("1. Receive with 0 flag (blocking)\n");
        printf("2. Receive with IPC_NOWAIT flag (non-blocking)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            printf("\nWaiting for message with 0 flag (blocking mode)...\n");
            printf("Run sender program to send a message\n");

            if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1)
            {
                perror("msgrcv with 0 flag failed");
            }
            else
            {
                printf("Received message: %s\n", msg.mtext);
            }
            break;

        case 2:
            printf("\nChecking for message with IPC_NOWAIT flag (non-blocking mode)...\n");

            ssize_t result = msgrcv(msgid, &msg, sizeof(msg.mtext), 1, IPC_NOWAIT);

            if (result == -1)
            {
                if (errno == ENOMSG)
                {
                    printf("No message available in queue\n");
                }
                else
                {
                    perror("msgrcv with IPC_NOWAIT flag failed");
                }
            }
            else
            {
                printf("Received message: %s\n", msg.mtext);
            }
            break;

        case 3:
            if (msgctl(msgid, IPC_RMID, NULL) == -1)
            {
                perror("msgctl (remove) failed");
            }
            else
            {
                printf("Message queue removed successfully\n");
            }
            exit(0);

        default:
            printf("Invalid choice! Please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P27$ gcc 27r.c -o 27r

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P27$ ./27r
Message queue created/accessed with ID: 4

=== Message Receiver Options ===
1. Receive with 0 flag (blocking)
2. Receive with IPC_NOWAIT flag (non-blocking)
3. Exit
Enter your choice: 1

Waiting for message with 0 flag (blocking mode)...
Run sender program to send a message
Received message: This is Software Sysem Practical 1.

---------------------------------------------------------------------------

=== Message Receiver Options ===
1. Receive with 0 flag (blocking)
2. Receive with IPC_NOWAIT flag (non-blocking)
3. Exit
Enter your choice: 2

Checking for message with IPC_NOWAIT flag (non-blocking mode)...
Received message: This is Software Sysem Practical 2.

============================================================================
*/
