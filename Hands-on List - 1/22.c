/*
============================================================================
Name : 22.c
Author : Shikhar Mutta
Description : Write a program, open a file, call fork, and then write to the file by both the child as well as the parent processes. Check output of the file.
Date: 24th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int fd;
    pid_t pid;
    fd = open("output.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0)
    {
        perror("Failed to open file");
        return 1;
    }
    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        close(fd);
        return 1;
    }
    else if (pid == 0)
    {

        char child_text[] = "Child writing to file\n";
        write(fd, child_text, strlen(child_text));
        printf("Child process wrote to file.\n");
    }
    else
    {

        char parent_text[] = "Parent writing to file\n";
        write(fd, parent_text, strlen(parent_text));
        printf("Parent process wrote to file.\n");
    }

    close(fd);
    return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P22$ gcc 22.c -o 22

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P22$ ./22

=> Check the file:
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P22$ cat output.txt
Parent writing to file
Child writing to file

============================================================================
*/