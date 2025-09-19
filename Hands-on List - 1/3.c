/*
============================================================================
Name : 3.c
Author : Shikhar Mutta
Description : Write a program to create a file and print the file descriptor value. Use creat ( ) system call
Date: 20th Aug, 2025.
============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  int fd;
  fd = creat("myfile.txt", 0644);
  if (fd == -1)
  {
    perror("creat");
    exit(1);
  }
  printf("File created successfully. File descriptor: %d\n", fd);
  close(fd);
  return 0;
}

/*
============================================================================
Compilation :
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P3$ gcc 3.c -o 3
shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P3$ ./3
File created successfully. File descriptor: 3

============================================================================
*/
