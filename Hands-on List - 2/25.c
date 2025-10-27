/*
============================================================================
Name : 25.c
Author : Shikhar Mutta
Description : Write a program to print a message queue's (use msqid_ds and ipc_perm structures)
                    a. access permission
                    b. uid, gid
                    c. time of last message sent and received
                    d. time of last change in the message queue
                    d. size of the queue
                    f. number of messages in the queue
                    g. maximum number of bytes allowed
                    h. pid of the msgsnd and msgrcv
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

int main(void)
{
    key_t key;
    int msqid;
    struct msqid_ds buf;

    key = ftok(".", 'A');
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1)
    {
        perror("msgget");
        exit(1);
    }

    if (msgctl(msqid, IPC_STAT, &buf) == -1)
    {
        perror("msgctl IPC_STAT");
        exit(1);
    }

    printf("Message Queue ID: %d\n", msqid);
    printf("Key: %d\n", key);

    printf("Access permissions: %o\n", buf.msg_perm.mode & 0777);

    printf("UID: %d\n", buf.msg_perm.uid);
    printf("GID: %d\n", buf.msg_perm.gid);

    printf("Last msgsnd time: %s", buf.msg_stime ? ctime(&buf.msg_stime) : "Not yet sent\n");
    printf("Last msgrcv time: %s", buf.msg_rtime ? ctime(&buf.msg_rtime) : "Not yet received\n");

    printf("Last change time: %s", ctime(&buf.msg_ctime));

#ifdef __linux__
    printf("Current number of bytes in queue: %lu\n", buf.__msg_cbytes);
#else
    printf("Current number of bytes in queue: %lu\n", buf.msg_cbytes);
#endif

    printf("Number of messages in queue: %lu\n", buf.msg_qnum);

    printf("Max bytes allowed: %lu\n", buf.msg_qbytes);

    printf("PID of last msgsnd: %d\n", buf.msg_lspid);
    printf("PID of last msgrcv: %d\n", buf.msg_lrpid);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P25$ gcc -o 25 25.c

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P25$ ./25
Message Queue ID: 1
Key: 1090860321
Access permissions: 666
UID: 1000
GID: 1000
Last msgsnd time: Not yet sent
Last msgrcv time: Not yet received
Last change time: Fri Sep 19 14:28:48 2025
Current number of bytes in queue: 0
Number of messages in queue: 0
Max bytes allowed: 16384
PID of last msgsnd: 0
PID of last msgrcv: 0

============================================================================
*/
