/*
============================================================================
Name : 6.c
Author : Shikhar Mutta
Description : Write a simple program to create three threads.
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *worker(void *arg)
{
    int id = *(int *)arg;
    printf("Thread %d: started (pthread_t = %lu)\n", id, (unsigned long)pthread_self());
    sleep(1);
    printf("Thread %d: finished\n", id);
    return NULL;
}

int main(void)
{
    pthread_t threads[3];
    int ids[3];

    for (int i = 0; i < 3; ++i)
    {
        ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, worker, &ids[i]) != 0)
        {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join");
            return 1;
        }
    }

    printf("All threads joined. Main exiting.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P6$ gcc 6.c -o 6

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P6$ ./6
Thread 2: started (pthread_t = 130711822526144)
Thread 1: started (pthread_t = 130711830918848)
Thread 3: started (pthread_t = 130711814133440)
Thread 2: finished
Thread 1: finished
Thread 3: finished

============================================================================
*/
