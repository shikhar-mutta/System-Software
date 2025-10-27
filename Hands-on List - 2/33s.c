/*
============================================================================
Name : 33s.c
Author : Shikhar Mutta
Description : Write a program to communicate between two machines using socket.
Date: 20th September 2025.
============================================================================
*/

// server.c
// Simple two-way TCP chat server using select()
// Usage: ./server <port>
// Example: ./server 12345

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUF_SZ 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) { perror("bind"); return 1; }
    if (listen(listen_fd, 1) < 0) { perror("listen"); return 1; }

    printf("Server listening on port %d. Waiting for a client...\n", port);

    struct sockaddr_in cli;
    socklen_t cli_len = sizeof(cli);
    int conn_fd = accept(listen_fd, (struct sockaddr*)&cli, &cli_len);
    if (conn_fd < 0) { perror("accept"); return 1; }

    char cli_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cli.sin_addr, cli_ip, sizeof(cli_ip));
    printf("Client connected from %s:%d\n", cli_ip, ntohs(cli.sin_port));

    fd_set readfds;
    int maxfd = (conn_fd > STDIN_FILENO) ? conn_fd : STDIN_FILENO;
    char buf[BUF_SZ];

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(conn_fd, &readfds);

        int nready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nready < 0) {
            if (errno == EINTR) continue;
            perror("select"); break;
        }

        // Input from keyboard -> send to client
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (!fgets(buf, sizeof(buf), stdin)) {
                // EOF on stdin -> close connection and exit
                printf("EOF on stdin, closing connection.\n");
                break;
            }
            // If user types "exit\n" then break
            if (strcmp(buf, "exit\n") == 0) {
                printf("Exiting as requested.\n");
                break;
            }
            size_t tosend = strlen(buf);
            ssize_t sent = send(conn_fd, buf, tosend, 0);
            if (sent <= 0) { perror("send"); break; }
        }

        // Data from client -> print to stdout
        if (FD_ISSET(conn_fd, &readfds)) {
            ssize_t recvd = recv(conn_fd, buf, sizeof(buf)-1, 0);
            if (recvd < 0) { perror("recv"); break; }
            if (recvd == 0) {
                printf("Client disconnected.\n");
                break;
            }
            buf[recvd] = '\0';
            printf("Client: %s", buf);
            fflush(stdout);
        }
    }

    close(conn_fd);
    close(listen_fd);
    return 0;
}


/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P33$ gcc 33s.c -o 33s

2. Run
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Upload Hands-on list 2/P33$ ./33s 12345
Server listening on port 12345. Waiting for a client...
Client connected from 127.0.0.1:34636
Client: Hey, I am client.
Client: What kind of services you can provide?
I can do searching content for you.
I can find books for you.

============================================================================
*/
