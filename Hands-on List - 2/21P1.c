/*
============================================================================
Name : 21P1.c
Author : Shikhar Mutta
Description : Write two programs so that both can communicate by FIFO -Use two way communications.
Date: 19th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>

#define FIFO1 "/tmp/fifo_server_to_client"
#define FIFO2 "/tmp/fifo_client_to_server"
#define BUFFER_SIZE 1024

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig)
{
    keep_running = 0;
}

int main()
{
    int fd_write, fd_read;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[2];

    signal(SIGINT, handle_signal);

    if (mkfifo(FIFO1, 0666) == -1 && errno != EEXIST)
    {
        perror("mkfifo server_to_client failed");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(FIFO2, 0666) == -1 && errno != EEXIST)
    {
        perror("mkfifo client_to_server failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for client connection...\n");

    fd_read = open(FIFO2, O_RDONLY | O_NONBLOCK);
    if (fd_read == -1)
    {
        perror("open FIFO2 for reading failed");
        exit(EXIT_FAILURE);
    }

    fd_write = open(FIFO1, O_WRONLY);
    if (fd_write == -1)
    {
        perror("open FIFO1 for writing failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected! You can send messages anytime.\n");
    printf("Type your messages (press Enter to send, 'exit' to quit):\n\n");

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = fd_read;
    fds[1].events = POLLIN;

    while (keep_running)
    {
        int ret = poll(fds, 2, 100);

        if (ret == -1)
        {
            perror("poll failed");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
            {

                buffer[strcspn(buffer, "\n")] = 0;

                if (strcmp(buffer, "exit") == 0)
                {
                    printf("Exit requested. Closing connection.\n");
                    write(fd_write, "exit\n", 5);
                    break;
                }

                if (strlen(buffer) > 0)
                {
                    strcat(buffer, "\n");
                    write(fd_write, buffer, strlen(buffer));
                    printf("You sent: %s", buffer);
                }
            }
        }

        if (fds[1].revents & POLLIN)
        {
            ssize_t bytes_read = read(fd_read, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0)
            {
                buffer[bytes_read] = '\0';

                if (strcmp(buffer, "exit\n") == 0)
                {
                    printf("Client requested exit. Closing connection.\n");
                    break;
                }

                printf("Client: %s", buffer);
            }
        }
    }

    close(fd_read);
    close(fd_write);
    unlink(FIFO1);
    unlink(FIFO2);

    printf("Server terminated.\n");
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P21$ gcc -o 21P1 21P1.c

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P21$ ./21P1
Server started. Waiting for client connection...
Client connected! You can send messages anytime.
Type your messages (press Enter to send, 'exit' to quit):

Client: Hey
How are you?
You sent: How are you?
Client: I am fine, What about you?
I am also fine.
You sent: I am also fine.


============================================================================
*/
