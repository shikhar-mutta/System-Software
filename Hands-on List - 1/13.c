/*
============================================================================
Name : 13.c
Author : Shikhar Mutta
Description : Write a program to wait for a STDIN for 10 seconds using select. Write a proper print statement to verify whether the data is available within 10 seconds or not (check in $man 2 select).
Date: 22th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

int main()
{
    fd_set readfds;
    struct timeval tv;
    int retval;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    printf("Waiting for input on STDIN (10 seconds timeout)...\n");
    fflush(stdout);

    retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        perror("select()");
        exit(EXIT_FAILURE);
    }
    else if (retval == 0)
    {
        printf("No data within 10 seconds.\n");
    }
    else
    {
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            printf("Data is available on STDIN! Reading now...\n");

            char buf[1024];
            ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
            if (n > 0)
            {
                buf[n] = '\0';
                printf("You entered: %s\n", buf);
            }
        }
    }

    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P13$ gcc 13.c -o 13

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P13$ ./13
Waiting for input on STDIN (10 seconds timeout)...
Shikhar
Data is available on STDIN! Reading now...
You entered: Shikhar

============================================================================
*/