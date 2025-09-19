/*
============================================================================
Name : 5.c
Author : Shikhar Mutta
Description : Write a program to create five new files with infinite loop. Execute the program in the background and check the file descriptor table at /proc/pid/fd.
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>  
#include <unistd.h> 
#include <stdlib.h> 

int main()
{
    int fd[5];
    char filename[20];

    
    for (int i = 0; i < 5; i++)
    {
        
        snprintf(filename, sizeof(filename), "file%d.txt", i + 1);
        fd[i] = open(filename, O_CREAT | O_RDWR, 0644);
        if (fd[i] == -1)
        {
            perror("open"); 
            exit(1);
        }
        printf("Created %s with file descriptor: %d\n", filename, fd[i]);
    }

    printf("Process PID: %d\n", getpid());

    
    while (1)
    {
        sleep(10); 
    }

    return 0;
}

/*
============================================================================
Compilation :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5$ gcc 5.c -o 5
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5$ ./5 &
[1] 40877
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5$ Created file1.txt with file descriptor: 3
Created file2.txt with file descriptor: 4
Created file3.txt with file descriptor: 5
Created file4.txt with file descriptor: 6
Created file5.txt with file descriptor: 7
Process PID: 40877

File Descriptor:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5$ ls -l /proc/40877/fd
total 0
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 0 -> /dev/pts/0
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 1 -> /dev/pts/0
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 2 -> /dev/pts/0
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 3 -> '/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5/file1.txt'
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 4 -> '/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5/file2.txt'
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 5 -> '/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5/file3.txt'
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 6 -> '/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5/file4.txt'
lrwx------ 1 shikhar shikhar 64 Aug 20 05:05 7 -> '/home/shikhar/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P5/file5.txt'

============================================================================
*/

