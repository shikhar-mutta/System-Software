/*
============================================================================
Name : 26.c
Author : Shikhar Mutta
Description : Write a program to send messages to the message queue. Check $ipcs -q
Date: 20th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PROJECT_ID 'A'
#define MAX_TEXT 1024

struct mymsg
{
    long mtype;
    char mtext[MAX_TEXT];
};

int main(int argc, char *argv[])
{
    key_t key;
    int msqid;
    struct mymsg msg;
    char line[MAX_TEXT];
    long mtype = 1;

    if (argc >= 2)
    {
        mtype = strtol(argv[1], NULL, 10);
        if (mtype <= 0)
        {
            fprintf(stderr, "Invalid message type '%s'\n", argv[1]);
            return 1;
        }
    }

    key = ftok(".", PROJECT_ID);
    if (key == (key_t)-1)
    {
        perror("ftok");
        return 1;
    }

    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1)
    {
        perror("msgget");
        return 1;
    }

    printf("Using message queue: key=%d  msqid=%d  (mtype=%ld)\n", (int)key, msqid, mtype);
    printf("Type lines to send. Type 'exit' to quit.\n");

    while (fgets(line, sizeof line, stdin) != NULL)
    {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            --len;
        }

        if (strcmp(line, "exit") == 0)
            break;

        msg.mtype = mtype;
        strncpy(msg.mtext, line, sizeof msg.mtext - 1);
        msg.mtext[sizeof msg.mtext - 1] = '\0';

        if (msgsnd(msqid, &msg, (size_t)strlen(msg.mtext) + 1, 0) == -1)
        {
            perror("msgsnd");
        }
        else
        {
            printf("Sent (%zu bytes): %s\n", strlen(msg.mtext) + 1, msg.mtext);
        }
    }

    printf("Sender exiting.\n");
    return 0;
}

/*
============================================================================
Compilation :

In terminal A:
1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P26$ gcc 26.c -o 26

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P26$ ./26
Using message queue: key=1090851335  msqid=3  (mtype=1)
Type lines to send. Type 'exit' to quit.
This is software system practicals
Sent (35 bytes): This is software system practicals
This is related to IPC.
Sent (24 bytes): This is related to IPC.

In terminal B (Validation):
1. To check the queue content in another terminal
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P26$ ipcs -q

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x41051207 3          shikhar    666        59           2    


============================================================================
*/
