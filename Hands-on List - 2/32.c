/*
============================================================================
Name : 32.c
Author : Shikhar Mutta
Description : Write a program to implement semaphore to protect any critical section.
                    a. rewrite the ticket number creation program using semaphore
                    b. protect shared memory from concurrent write access
                    c. protect multiple pseudo resources ( may be two) using counting semaphore
                    d. remove the created semaphore
Date: 20th September 2025.
============================================================================
*/

#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define PATH "/tmp"
#define PROJ_MUTEX 'M'
#define PROJ_COUNT 'C'
#define PROJ_SHM 'S'
#define SHM_SIZE 4096

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

static void msleep(long ms)
{
    if (ms <= 0)
        return;
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
    {
    }
}

int sem_P(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_V(int semid, int semnum)
{
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = 1;
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int main(void)
{
    key_t key_mutex, key_count, key_shm;
    int semid_mutex = -1, semid_count = -1, shmid = -1;
    union semun arg;
    int *ticket_counter = NULL;
    char *shmaddr = NULL;
    pid_t pid;
    int i;

    srand((unsigned)(time(NULL) ^ getpid()));

    key_mutex = ftok(PATH, PROJ_MUTEX);
    key_count = ftok(PATH, PROJ_COUNT);
    key_shm = ftok(PATH, PROJ_SHM);
    if (key_mutex == (key_t)-1 || key_count == (key_t)-1 || key_shm == (key_t)-1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    semid_mutex = semget(key_mutex, 1, IPC_CREAT | 0666);
    if (semid_mutex == -1)
    {
        perror("semget mutex");
        exit(EXIT_FAILURE);
    }
    arg.val = 1;
    if (semctl(semid_mutex, 0, SETVAL, arg) == -1)
    {
        perror("semctl SETVAL mutex");
        exit(EXIT_FAILURE);
    }
    printf("[setup] Created mutex semaphore: semid=%d (initial=1)\n", semid_mutex);

    semid_count = semget(key_count, 1, IPC_CREAT | 0666);
    if (semid_count == -1)
    {
        perror("semget count");
        exit(EXIT_FAILURE);
    }
    arg.val = 2;
    if (semctl(semid_count, 0, SETVAL, arg) == -1)
    {
        perror("semctl SETVAL count");
        exit(EXIT_FAILURE);
    }
    printf("[setup] Created counting semaphore: semid=%d (initial=2)\n", semid_count);

    shmid = shmget(key_shm, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shmaddr = (char *)shmat(shmid, NULL, 0);
    if (shmaddr == (char *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    ticket_counter = (int *)shmaddr;
    *ticket_counter = 0;
    printf("[setup] Created shared memory: shmid=%d, ticket_counter=%d\n", shmid, *ticket_counter);

    printf("\n=== (a) Ticket generation with mutex protection ===\n");
    for (i = 0; i < 6; ++i)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            break;
        }
        if (pid == 0)
        {
            msleep(rand() % 200);
            if (sem_P(semid_mutex, 0) == -1)
            {
                fprintf(stderr, "[child %d] sem_P(mutex) failed: %s\n", getpid(), strerror(errno));
                _exit(1);
            }
            int t = *ticket_counter;
            msleep(100);
            t += 1;
            *ticket_counter = t;
            printf("[ticket] child pid=%d got ticket #%d (shmid=%d)\n", getpid(), t, shmid);
            if (sem_V(semid_mutex, 0) == -1)
            {
                fprintf(stderr, "[child %d] sem_V(mutex) failed: %s\n", getpid(), strerror(errno));
                _exit(2);
            }
            _exit(0);
        }
    }
    for (i = 0; i < 6; ++i)
        wait(NULL);

    printf("[parent] ticket_counter after generation = %d\n", *ticket_counter);

    printf("\n=== (b) Protecting shared memory writes with mutex ===\n");
    for (i = 0; i < 2; ++i)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            break;
        }
        if (pid == 0)
        {
            const char *texts[2] = {"Writer A was here.", "Writer B overwrote."};
            msleep(rand() % 200);
            if (sem_P(semid_mutex, 0) == -1)
            {
                fprintf(stderr, "[writer %d] sem_P failed: %s\n", getpid(), strerror(errno));
                _exit(1);
            }
            snprintf(shmaddr + sizeof(int), SHM_SIZE - sizeof(int), "%s (pid=%d)", texts[i], getpid());
            printf("[writer] pid=%d wrote protected string to shmid=%d\n", getpid(), shmid);
            msleep(1000); 
            if (sem_V(semid_mutex, 0) == -1)
            {
                fprintf(stderr, "[writer %d] sem_V failed: %s\n", getpid(), strerror(errno));
                _exit(2);
            }
            _exit(0);
        }
    }
    for (i = 0; i < 2; ++i)
        wait(NULL);

    printf("[parent] Shared memory content (after protected writes): \"%s\"\n", shmaddr + sizeof(int));

    printf("\n=== (c) Counting semaphore (2 resources) demo ===\n");
    for (i = 0; i < 5; ++i)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            break;
        }
        if (pid == 0)
        {
            msleep(rand() % 300);
            printf("[user] pid=%d attempting to acquire resource (semid_count=%d)\n", getpid(), semid_count);
            if (sem_P(semid_count, 0) == -1)
            {
                fprintf(stderr, "[user %d] sem_P(count) failed: %s\n", getpid(), strerror(errno));
                _exit(1);
            }
            printf("[user] pid=%d acquired resource (shmid=%d). Using it...\n", getpid(), shmid);
            msleep(1000 + (rand() % 2000));
            printf("[user] pid=%d releasing resource (shmid=%d)\n", getpid(), shmid);
            if (sem_V(semid_count, 0) == -1)
            {
                fprintf(stderr, "[user %d] sem_V(count) failed: %s\n", getpid(), strerror(errno));
                _exit(2);
            }
            _exit(0);
        }
    }
    for (i = 0; i < 5; ++i)
        wait(NULL);

    printf("\n=== (d) Cleanup: detach and remove semaphores & shared memory ===\n");

    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt parent");
    }
    else
    {
        printf("[cleanup] Parent detached shared memory (shmid=%d)\n", shmid);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl(IPC_RMID)");
    }
    else
    {
        printf("[cleanup] Removed shared memory: shmid=%d\n", shmid);
    }

    if (semctl(semid_mutex, 0, IPC_RMID) == -1)
    {
        perror("semctl IPC_RMID mutex");
    }
    else
    {
        printf("[cleanup] Removed mutex semaphore: semid=%d\n", semid_mutex);
    }

    if (semctl(semid_count, 0, IPC_RMID) == -1)
    {
        perror("semctl IPC_RMID count");
    }
    else
    {
        printf("[cleanup] Removed counting semaphore: semid=%d\n", semid_count);
    }

    printf("\nAll done.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P32$ gcc 32.c -o 32

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P32$ ./32
[setup] Created mutex semaphore: semid=4 (initial=1)
[setup] Created counting semaphore: semid=5 (initial=2)
[setup] Created shared memory: shmid=163841, ticket_counter=0

=== (a) Ticket generation with mutex protection ===
[ticket] child pid=15788 got ticket #1 (shmid=163841)
[ticket] child pid=15789 got ticket #2 (shmid=163841)
[ticket] child pid=15790 got ticket #3 (shmid=163841)
[ticket] child pid=15791 got ticket #4 (shmid=163841)
[ticket] child pid=15792 got ticket #5 (shmid=163841)
[ticket] child pid=15793 got ticket #6 (shmid=163841)
[parent] ticket_counter after generation = 6

=== (b) Protecting shared memory writes with mutex ===
[writer] pid=15794 wrote protected string to shmid=163841
[writer] pid=15795 wrote protected string to shmid=163841
[parent] Shared memory content (after protected writes): "Writer B overwrote. (pid=15795)"

=== (c) Counting semaphore (2 resources) demo ===
[user] pid=15797 attempting to acquire resource (semid_count=5)
[user] pid=15797 acquired resource (shmid=163841). Using it...
[user] pid=15798 attempting to acquire resource (semid_count=5)
[user] pid=15798 acquired resource (shmid=163841). Using it...
[user] pid=15799 attempting to acquire resource (semid_count=5)
[user] pid=15800 attempting to acquire resource (semid_count=5)
[user] pid=15801 attempting to acquire resource (semid_count=5)
[user] pid=15797 releasing resource (shmid=163841)
[user] pid=15799 acquired resource (shmid=163841). Using it...
[user] pid=15798 releasing resource (shmid=163841)
[user] pid=15800 acquired resource (shmid=163841). Using it...
[user] pid=15799 releasing resource (shmid=163841)
[user] pid=15800 releasing resource (shmid=163841)
[user] pid=15801 acquired resource (shmid=163841). Using it...
[user] pid=15801 releasing resource (shmid=163841)

=== (d) Cleanup: detach and remove semaphores & shared memory ===
[cleanup] Parent detached shared memory (shmid=163841)
[cleanup] Removed shared memory: shmid=163841
[cleanup] Removed mutex semaphore: semid=4
[cleanup] Removed counting semaphore: semid=5

All done.

============================================================================
*/
