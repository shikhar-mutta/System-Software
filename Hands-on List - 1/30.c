/*
============================================================================
Name : 30.c
Author : Shikhar Mutta
Description : Write a program to run a script at a specific time using a Daemon process.
Date: 25th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

int main()
{
    pid_t pid, sid;
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {

        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0)
    {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    while (1)
    {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        int target_hour = t->tm_hour;
        int target_minute = (t->tm_min + 1) % 60;
        if (t->tm_hour == target_hour && t->tm_min == target_minute)
        {
            system("/home/shikhar/30myscript.sh");
            sleep(60);
        }
        else
        {
            sleep(10);
        }
    }
    return 0;
}


/*
============================================================================
Compilation :
=> Create the Script (  MOVE SCRIPT AT ROOT DIR /30myscript.sh  )
Save this as 30myscript.sh in your working directory:
#!/bin/bash
# Log the execution time
echo "Script ran at $(date)" >> /tmp/daemon_test.log

=> Make it executable:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P30$ chmod +x ~/30myscript.sh


shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P30$ gcc 30.c -o 30

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P30$ sudo ./30

=> Check the Log

After the target time (one minute ahead for testing), check:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P30$ cat /tmp/daemon_test.log
Script ran at Wed Aug 20 02:26:33 PM IST 2025

============================================================================
*/
