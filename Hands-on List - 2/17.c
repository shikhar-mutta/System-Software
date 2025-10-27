/*
============================================================================
Name : 17.c
Author : Shikhar Mutta
Description : Write a program to execute ls -l | wc.
                    a. use dup
                    b. use dup2
                    c. use fcntl
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

static void run_with_dup(void) {
    int pipefd[2];
    pid_t c1, c2;

    if (pipe(pipefd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    c1 = fork();
    if (c1 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c1 == 0) {
        close(pipefd[0]);
        if (close(STDOUT_FILENO) == -1 && errno != EBADF) perror("close stdout");
        if (dup(pipefd[1]) == -1) { perror("dup"); _exit(127); }
        close(pipefd[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp ls"); _exit(127);
    }

    c2 = fork();
    if (c2 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c2 == 0) {
        close(pipefd[1]);
        if (close(STDIN_FILENO) == -1 && errno != EBADF) perror("close stdin");
        if (dup(pipefd[0]) == -1) { perror("dup"); _exit(127); }
        close(pipefd[0]);
        execlp("wc", "wc", (char *)NULL);
        perror("execlp wc"); _exit(127);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
}

static void run_with_dup2(void) {
    int pipefd[2];
    pid_t c1, c2;

    if (pipe(pipefd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    c1 = fork();
    if (c1 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c1 == 0) {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) { perror("dup2"); _exit(127); }
        close(pipefd[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp ls"); _exit(127);
    }

    c2 = fork();
    if (c2 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c2 == 0) {
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1) { perror("dup2"); _exit(127); }
        close(pipefd[0]);
        execlp("wc", "wc", (char *)NULL);
        perror("execlp wc"); _exit(127);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
}

static void run_with_fcntl(void) {
    int pipefd[2];
    pid_t c1, c2;

    if (pipe(pipefd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    c1 = fork();
    if (c1 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c1 == 0) {
        close(pipefd[0]);
        if (close(STDOUT_FILENO) == -1 && errno != EBADF) perror("close stdout");
        if (fcntl(pipefd[1], F_DUPFD, STDOUT_FILENO) == -1) { perror("fcntl F_DUPFD"); _exit(127); }
        close(pipefd[1]);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp ls"); _exit(127);
    }

    c2 = fork();
    if (c2 < 0) { perror("fork"); exit(EXIT_FAILURE); }

    if (c2 == 0) {
        close(pipefd[1]);
        if (close(STDIN_FILENO) == -1 && errno != EBADF) perror("close stdin");
        if (fcntl(pipefd[0], F_DUPFD, STDIN_FILENO) == -1) { perror("fcntl F_DUPFD"); _exit(127); }
        close(pipefd[0]);
        execlp("wc", "wc", (char *)NULL);
        perror("execlp wc"); _exit(127);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
}

int main(void) {
    printf("== Running Method: dup ==\n");
    run_with_dup();

    printf("== Running Method: dup2 ==\n");
    run_with_dup2();

    printf("== Running Method: fcntl (F_DUPFD) ==\n");
    run_with_fcntl();

    return EXIT_SUCCESS;
}


/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P17$ gcc 17.c -o 17

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P17$ ./17
== Running Method: dup ==
      3      20     113
== Running Method: dup2 ==
      3      20     113
== Running Method: fcntl (F_DUPFD) ==
      3      20     113

============================================================================
*/
