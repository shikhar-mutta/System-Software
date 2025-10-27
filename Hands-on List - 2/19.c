/*
============================================================================
Name : 19.c
Author : Shikhar Mutta
Description : Create a FIFO file by
                    a. mknod command
                    b. mkfifo command
                    c. use strace command to find out, which command (mknod or mkfifo) is better.
                    c. mknod system call
                    d. mkfifo library function
Date: 19th September 2025.
============================================================================
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SZ 256

static void strip_newline(char *s)
{
    size_t n = strlen(s);
    if (n == 0)
        return;
    if (s[n - 1] == '\n')
        s[n - 1] = '\0';
}

int main(void)
{
    char line[BUF_SZ];
    int choice = 0;
    char name[BUF_SZ];
    char path[BUF_SZ + 10];

    printf("Choose method to create FIFO in current directory (PWD):\n");
    printf("  1) mknod(2)\n");
    printf("  2) mkfifo(3)\n");
    printf("Enter choice (1 or 2): ");
    if (!fgets(line, sizeof(line), stdin))
    {
        fprintf(stderr, "Input error\n");
        return 1;
    }
    strip_newline(line);
    choice = atoi(line);

    if (choice != 1 && choice != 2)
    {
        fprintf(stderr, "Invalid choice. Use 1 or 2.\n");
        return 2;
    }

    printf("Enter FIFO filename (relative or absolute): ");
    if (!fgets(name, sizeof(name), stdin))
    {
        fprintf(stderr, "Input error\n");
        return 1;
    }
    strip_newline(name);
    if (name[0] == '\0')
    {
        fprintf(stderr, "Empty filename.\n");
        return 2;
    }

    if (name[0] == '/')
    {
        strncpy(path, name, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
    }
    else
    {
        if (snprintf(path, sizeof(path), "./%s", name) >= (int)sizeof(path))
        {
            fprintf(stderr, "Filename too long\n");
            return 1;
        }
    }

    if (choice == 1)
    {
        mode_t mode = S_IFIFO | 0666;
        if (mknod(path, mode, 0) == -1)
        {
            fprintf(stderr, "mknod(%s) failed: %s\n", path, strerror(errno));
            return 1;
        }
        printf("Created FIFO via mknod(): %s\n", path);
    }
    else
    {
        mode_t mode = 0666;
        if (mkfifo(path, mode) == -1)
        {
            fprintf(stderr, "mkfifo(%s) failed: %s\n", path, strerror(errno));
            return 1;
        }
        printf("Created FIFO via mkfifo(): %s\n", path);
    }

    printf("\nVerification (ls -l):\n");
    fflush(stdout);
    pid_t p = fork();
    if (p == -1)
    {
        perror("fork");
        return 1;
    }
    else if (p == 0)
    {
        execlp("ls", "ls", "-l", path, (char *)NULL);
        perror("execlp ls");
        _exit(127);
    }
    else
    {
        int status;
        waitpid(p, &status, 0);
    }

    return 0;
}

/*
============================================================================
Shell commands:

a) Using mknod (command-line)
1. create FIFO named fifo_mknod
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ mknod fifo_mknod p
2. check created or not
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ ls -l fifo_mknod
prw-rw-r-- 1 shikhar shikhar 0 Sep 19 12:27 fifo_mknod

b) Using mkfifo (command-line)
1. create FIFO named fifo_mkfifo
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ mkfifo fifo_mkfifo
2. check created or not
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ ls -l fifo_mkfifo
prw-rw-r-- 1 shikhar shikhar 0 Sep 19 12:28 fifo_mkfifo

c) use strace command to find out, which command (mknod or mkfifo) is better

# trace file-related syscalls for mkfifo command
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ strace -f -e trace=file -o trace_mkfifo_cmd.txt mkfifo fifo_strace_mkfifo


# trace file-related syscalls for mknod command
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ strace -f -e trace=file -o trace_mknod_cmd.txt mknod fifo_strace_mknod p

# view the traces
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ sed -n '1,200p' trace_mkfifo_cmd.txt
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ sed -n '1,200p' trace_mknod_cmd.txt

# or grep relevant syscalls
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ grep -E "mknod|mknodat|mkfifo|mkfifoat|open|stat" trace_mkfifo_cmd.txt trace_mknod_cmd.txt -n

Which is better?
    ->For creating FIFOs prefer mkfifo (command or mkfifo(3) in C). It’s the standard, clearer, and portable way to express “create a FIFO.”
    ->mknod is lower-level and most commonly used for device nodes (block/char devices) where you supply major/minor numbers. You can use mknod for FIFOs, but it’s less idiomatic.


For d) & f)
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ gcc 19.c -o 19

2. Run
mknod =>
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ ./19
Choose method to create FIFO in current directory (PWD):
  1) mknod(2)
  2) mkfifo(3)
Enter choice (1 or 2): 1
Enter FIFO filename (relative or absolute): fifo1
Created FIFO via mknod(): ./fifo1

Verification (ls -l):
prw-rw-r-- 1 shikhar shikhar 0 Sep 19 12:48 ./fifo1

mkfifo=>
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P19$ ./19
Choose method to create FIFO in current directory (PWD):
  1) mknod(2)
  2) mkfifo(3)
Enter choice (1 or 2): 2
Enter FIFO filename (relative or absolute): fifo2
Created FIFO via mkfifo(): ./fifo2

Verification (ls -l):
prw-rw-r-- 1 shikhar shikhar 0 Sep 19 12:49 ./fifo2

============================================================================
*/
