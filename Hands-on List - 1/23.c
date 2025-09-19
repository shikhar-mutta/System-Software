/*
============================================================================
Name : 23.c
Author : Shikhar Mutta
Description : Write a program to create a Zombie state of the running program.
Date: 24th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else if (pid == 0)
    {
        printf("Child process (PID = %d) is exiting...\n", getpid());
        exit(0);
    }
    else
    {
        printf("Parent process (PID = %d) sleeping...\n", getpid());
        printf("Child PID = %d will become a zombie until parent calls wait()\n", pid);
        sleep(30);
        printf("Parent exiting...\n");
    }
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P23$ gcc 23.c -o 23

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P23$ ./23
Parent process (PID = 11272) sleeping...
Child PID = 11273 will become a zombie until parent calls wait()
Child process (PID = 11273) is exiting...
Parent exiting...

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P23$ ps aux | grep Z
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
shikhar    11273  0.0  0.0      0     0 pts/1    Z+   09:03   0:00 [23] <defunct>
shikhar    11282  0.0  0.0   9144  2332 pts/2    S+   09:04   0:00 grep --color=auto Z

The child process will have Z in the STAT column (zombie).

============================================================================
*/

