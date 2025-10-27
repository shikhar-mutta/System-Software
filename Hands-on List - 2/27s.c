/*
============================================================================
Name : 27s.c
Author : Shikhar Mutta
Description : Write a program to receive messages from the message queue.
                    a. with 0 as a flag
                    b. with IPC_NOWAIT as a flag
Date: 20th September 2025.
============================================================================
*/

// Sender Program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_MSG_SIZE 1024
#define MSG_KEY 1234

struct message
{
    long mtype;
    char mtext[MAX_MSG_SIZE];
};

int main()
{
    int msgid;
    struct message msg;

    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1)
    {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to message queue ID: %d\n", msgid);

    msg.mtype = 1;
    printf("Enter message to send: ");
    fgets(msg.mtext, MAX_MSG_SIZE, stdin);

    msg.mtext[strcspn(msg.mtext, "\n")] = 0;

    if (msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0) == -1)
    {
        perror("msgsnd failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent successfully: '%s'\n", msg.mtext);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P27$ gcc 27s.c -o 27s

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P27$ ./27s
Connected to message queue ID: 4
Enter message to send: This is Software Sysem Practical 1.   
Message sent successfully: 'This is Software Sysem Practical 1.'

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P27$ ./27s
Connected to message queue ID: 4
Enter message to send: This is Software Sysem Practical 2.
Message sent successfully: 'This is Software Sysem Practical 2.'

============================================================================
*/
