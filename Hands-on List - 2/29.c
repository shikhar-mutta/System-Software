/*
============================================================================
Name : 29.c
Author : Shikhar Mutta
Description : Write a program to remove the message queue
Date: 20th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(void)
{
    char line[128];
    long msqid_long;
    int msqid;
    char *endptr;

    printf("Existing message queues (ipcs -q):\n");
    if (system("ipcs -q") == -1)
    {
        perror("system(ipcs -q) failed");
    }

    printf("\nEnter msqid to remove (empty to cancel): ");
    if (!fgets(line, sizeof(line), stdin))
    {
        printf("No input. Exiting.\n");
        return 0;
    }

    line[strcspn(line, "\n")] = '\0';

    if (line[0] == '\0')
    {
        printf("Cancelled by user. Exiting.\n");
        return 0;
    }

    errno = 0;
    msqid_long = strtol(line, &endptr, 10);
    if (endptr == line || *endptr != '\0' || errno != 0)
    {
        fprintf(stderr, "Invalid msqid: '%s'\n", line);
        return 1;
    }

    if (msqid_long < 0 || msqid_long > INT_MAX)
    {
        fprintf(stderr, "msqid out of range: %ld\n", msqid_long);
        return 1;
    }

    msqid = (int)msqid_long;

    printf("Remove message queue with msqid=%d ? (Y/N): ", msqid);
    if (!fgets(line, sizeof(line), stdin))
    {
        printf("No input. Exiting.\n");
        return 0;
    }
    if (line[0] != 'y' && line[0] != 'Y')
    {
        printf("Aborted.\n");
        return 0;
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl(IPC_RMID) failed");
        return 1;
    }

    printf("Message queue msqid=%d removed successfully.\n", msqid);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P29$ gcc 29.c -o 29

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P29$ ./29
Existing message queues (ipcs -q):

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x41051501 8          shikhar    644        0            0


Enter msqid to remove (empty to cancel): 8
Remove message queue with msqid=8 ? (Y/N): Y
Message queue msqid=8 removed successfully.

============================================================================
*/
