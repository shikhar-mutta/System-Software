/*
============================================================================
Name : 28.c
Author : Shikhar Mutta
Description : Write a program to change the exiting message queue permission. (use msqid_ds structure)
Date: 20th September 2025.
============================================================================
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    key_t key;
    int msqid;
    struct msqid_ds buf;

    key = ftok("/tmp", 65);
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    msqid = msgget(key, 0666 | IPC_CREAT);
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

    printf("Before change: \nmsqid - %d \npermissions - %o\n\n", msqid, buf.msg_perm.mode & 0777);

    buf.msg_perm.mode = (buf.msg_perm.mode & ~0777) | 0644;

    if (msgctl(msqid, IPC_SET, &buf) == -1)
    {
        perror("msgctl IPC_SET");
        exit(1);
    }

    if (msgctl(msqid, IPC_STAT, &buf) == -1)
    {
        perror("msgctl IPC_STAT (verify)");
        exit(1);
    }

    printf("After change : \nmsqid - %d \npermissions - %o\n", msqid, buf.msg_perm.mode & 0777);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P28$ gcc 28.c -o 28

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P28$ ./28
Before change:
msqid - 5
permissions - 644

After change :
msqid - 5
permissions - 644

============================================================================
*/
