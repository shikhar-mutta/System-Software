/*
============================================================================
Name : 20.c
Author : Shikhar Mutta
Description : Find out the priority of your running program. Modify the priority with nice command.
Date: 23th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Running process with PID: %d\n", getpid());
    while (1) {
        sleep(5); // keep alive so you can inspect it
    }
    return 0;
}

/*
============================================================================
Compilation :
=> 1. Check the priority of a running program:
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P20$ gcc 20.c -o 20

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P20$ ./20 &
[1] 16730

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P20$ Running process with PID: 16730

shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P20$ ps -o pid,pri,ni,comm -p 19069
    PID PRI  NI COMMAND
  16730  19   0 20
# The PR column shows actual priority.

=> 2. Modify the priority with nice and renice
 => Start a program with a specific priority: 
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 1/P20$ nice -n 10 ./20
Running process with PID: 16805

# priority value: from -20 (highest priority) to +19 (lowest priority)

============================================================================
*/