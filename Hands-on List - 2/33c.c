/*
============================================================================
Name : 33c.c
Author : Shikhar Mutta
Description : Write a program to create a message queue and print the key and message queue id.
Date: 20th September 2025.
============================================================================
*/

// client.c
// Simple two-way TCP chat client using select()
// Usage: ./client <server-ip> <port>
// Example: ./client 127.0.0.1 12345

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUF_SZ 4096

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server-ip> <port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &srv.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    {
        perror("connect");
        return 1;
    }

    printf("Connected to %s:%d. Type messages and press Enter. Type 'exit' to quit.\n", server_ip, port);

    fd_set readfds;
    int maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;
    char buf[BUF_SZ];

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        int nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nready < 0)
        {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        // Keyboard input -> send to server
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (!fgets(buf, sizeof(buf), stdin))
            {
                printf("EOF on stdin, closing.\n");
                break;
            }
            if (strcmp(buf, "exit\n") == 0)
            {
                printf("Exiting as requested.\n");
                break;
            }
            size_t tosend = strlen(buf);
            ssize_t sent = send(sock, buf, tosend, 0);
            if (sent <= 0)
            {
                perror("send");
                break;
            }
        }

        // Data from server -> print to stdout
        if (FD_ISSET(sock, &readfds))
        {
            ssize_t recvd = recv(sock, buf, sizeof(buf) - 1, 0);
            if (recvd < 0)
            {
                perror("recv");
                break;
            }
            if (recvd == 0)
            {
                printf("Server closed connection.\n");
                break;
            }
            buf[recvd] = '\0';
            printf("Server: %s", buf);
            fflush(stdout);
        }
    }

    close(sock);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P33$ gcc 33c.c -o 33c

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P33$ ./33c 127.0.0.1 12345 
Connected to 127.0.0.1:12345. Type messages and press Enter. Type 'exit' to quit.
Hey, I am client.
What kind of services you can provide?
Server: I can do searching content for you.
Server: I can find books for you.

============================================================================
*/
