/*
============================================================================
Name : 7.c
Author : Shikhar Mutta
Description : Write a simple program to print the created thread ids.
Date: 18th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *worker(void *arg)
{
    printf("Hello from thread, pthread id = %lu\n", (unsigned long)pthread_self());
    return NULL;
}

int main(void)
{
    pthread_t threads[3];

    for (int i = 0; i < 3; i++)
    {
        if (pthread_create(&threads[i], NULL, worker, NULL) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P7$ gcc 7.c -o 7

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P7$ ./7
Hello from thread, pthread id = 127921339496128
Hello from thread, pthread id = 127921331103424
Hello from thread, pthread id = 127921186404032
All threads finished.

============================================================================
*/
