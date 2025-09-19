/*
============================================================================
Name : 6.c
Author : Shikhar Mutta
Description : Write a program to create five new files with infinite loop. Execute the program in the background and check the file descriptor table at /proc/pid/fd.
Date: 21th Aug, 2025.
============================================================================
*/

#include <unistd.h>
#include <stdlib.h>

int main()
{
    char buffer[100];
    int n;
    n = read(0, buffer, sizeof(buffer));
    if (n < 0)
    {
        write(2, "Error reading input\n", 20);
        exit(1);
    }
    write(1, buffer, n);
    return 0;
}

/*
============================================================================
Compilation :

shikhar@shikhar-pc:~/Sem 1/Software System/Softeware Sysyem /Hands-on list 1/P6$ gcc 6.c -o 6
./6
Software system is awesome subject!
Software system is awesome subject!

============================================================================
*/
