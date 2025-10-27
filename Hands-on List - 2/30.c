/*
============================================================================
Name : 30.c
Author : Shikhar Mutta
Description : Write a program to create a shared memory.
                    a. write some data to the shared memory
                    b. attach with O_RDONLY and check whether you are able to overwrite.
                    c. detach the shared memory
                    d. remove the shared memory
Date: 20th September 2025.
============================================================================
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_PATH "/tmp"
#define SHM_PROJ_ID 77
#define SHM_SIZE 1024

volatile sig_atomic_t saw_sigsegv = 0;
void sigsegv_handler(int sig)
{
    (void)sig;
    saw_sigsegv = 1;
    _exit(2);
}

int main(void)
{
    key_t key;
    int shmid;
    char *shmaddr = NULL;

    key = ftok(SHM_PATH, SHM_PROJ_ID);
    if (key == (key_t)-1)
    {
        perror("ftok");
        return 1;
    }

    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }
    printf("-------------------- create a shared memory -------------------- \n");
    printf("Created/obtained shared memory: shmid = %d\n\n", shmid);

    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1)
    {
        fprintf(stderr, "shmat (rw) failed for shmid = %d: %s\n", shmid, strerror(errno));
        return 1;
    }

    const char *msg = "Hello from parent (shared memory)";
    strncpy(shmaddr, msg, SHM_SIZE - 1);
    shmaddr[SHM_SIZE - 1] = '\0';

    printf("-------------------- Write some data to the shared memory -------------------- \n");
    printf("Parent wrote to shmid = %d : \"%s\"\n\n", shmid, shmaddr);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        if (shmdt(shmaddr) == -1)
        {
            fprintf(stderr, "shmdt (parent) failed for shmid = %d : %s \n\n", shmid, strerror(errno));
        }
        return 1;
    }

    if (pid == 0)
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sigsegv_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGSEGV, &sa, NULL);

        char *ro_addr = (char *)shmat(shmid, NULL, SHM_RDONLY);
        if (ro_addr == (char *)-1)
        {
            fprintf(stderr, "shmat (ro) in child failed for shmid = %d : %s \n\n", shmid, strerror(errno));
            _exit(1);
        }

        printf("-------------------- Attach with O_RDONLY and check whether you are able to overwrite.  -------------------- \n");

        printf("Child attached read-only to shmid = %d. \nContent: \"%s\"\n\n", shmid, ro_addr);

        printf("Child attempting to overwrite read-only mapping for shmid = %d...\n\n", shmid);
        ro_addr[0] = 'X';

        printf("Child: overwrite succeeded unexpectedly on shmid = %d. \nNew content: \"%s\"\n\n", shmid, ro_addr);

        if (shmdt(ro_addr) == -1)
        {
            fprintf(stderr, "shmdt (child) failed for shmid = %d: %s\n", shmid, strerror(errno));
        }
        _exit(0);
    }
    else
    {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);
            if (code == 0)
            {
                printf("Child exited normally for shmid = %d (no SIGSEGV). It may have overwritten (unexpected).\n", shmid);
            }
            else if (code == 1)
            {
                printf("Child failed to attach read-only or other error for shmid = %d (exit code 1).\n", shmid);
            }
            else if (code == 2)
            {
                printf("Child attempted write and triggered SIGSEGV for shmid = %d (expected behavior).\n\n", shmid);
            }
            else
            {
                printf("Child exited with code %d for shmid = %d.\n", code, shmid);
            }
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child terminated by signal %d for shmid = %d\n", WTERMSIG(status), shmid);
        }
        else
        {
            printf("Child ended with status %d for shmid = %d\n", status, shmid);
        }
    }

    if (shmdt(shmaddr) == -1)
    {
        fprintf(stderr, "shmdt (parent) failed for shmid = %d: %s\n\n", shmid, strerror(errno));
    }
    else
    {
        printf("-------------------- Detach the shared memory -------------------- \n");
        printf("Parent detached from shared memory shmid = %d.\n\n", shmid);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed for shmid = %d: %s\n\n", shmid, strerror(errno));
        return 1;
    }

    printf("-------------------- Remove the shared memory -------------------- \n");
    printf("Removed shared memory (shmid = %d).\n", shmid);

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P30$ gcc 30.c -o 30

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P30$ ./30
-------------------- create a shared memory -------------------- 
Created/obtained shared memory: shmid = 327708

-------------------- Write some data to the shared memory -------------------- 
Parent wrote to shmid = 327708 : "Hello from parent (shared memory)"

-------------------- Attach with O_RDONLY and check whether you are able to overwrite.  -------------------- 
Child attached read-only to shmid = 327708. 
Content: "Hello from parent (shared memory)"

Child attempting to overwrite read-only mapping for shmid = 327708...

Child attempted write and triggered SIGSEGV for shmid = 327708 (expected behavior).

-------------------- Detach the shared memory -------------------- 
Parent detached from shared memory shmid = 327708.

-------------------- Remove the shared memory -------------------- 
Removed shared memory (shmid = 327708).

============================================================================
*/
