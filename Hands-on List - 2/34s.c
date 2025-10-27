/*
============================================================================
Name : 34.c
Author : Shikhar Mutta
Description : Write a program to create a concurrent server.
                    a. use fork
                    b. use pthread_create
Date: 20th September 2025.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client_fork(int client_socket);
void *handle_client_thread(void *arg);
void fork_based_server(const char *ip_address, int port);
void pthread_based_server(const char *ip_address, int port);

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    int bytes_received;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_len);

    printf("Client connected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[bytes_received] = '\0';
        printf("Received from %s:%d: %s",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        send(client_socket, buffer, bytes_received, 0);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
    }

    printf("Client disconnected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    close(client_socket);
}

void handle_client_fork(int client_socket)
{

    handle_client(client_socket);
    exit(0);
}

void *handle_client_thread(void *arg)
{
    int client_socket = *((int *)arg);
    free(arg);

    handle_client(client_socket);
    pthread_exit(NULL);
}

void fork_based_server(const char *ip_address, int port)
{
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf("Starting fork-based concurrent server on %s:%d...\n", ip_address, port);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (strcmp(ip_address, "0.0.0.0") == 0)
    {
        address.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, ip_address, &address.sin_addr) <= 0)
        {
            perror("Invalid IP address");
            exit(EXIT_FAILURE);
        }
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Fork server listening on %s:%d...\n", ip_address, port);
    printf("Press Ctrl+C to stop the server\n");

    while (1)
    {

        if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork failed");
            close(client_socket);
        }
        else if (pid == 0)
        {
            close(server_fd);
            handle_client_fork(client_socket);
        }
        else
        {
            close(client_socket);

            while (waitpid(-1, NULL, WNOHANG) > 0)
                ;
        }
    }
}

void pthread_based_server(const char *ip_address, int port)
{
    int server_fd, *client_socket_ptr;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    printf("Starting pthread-based concurrent server on %s:%d...\n", ip_address, port);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (strcmp(ip_address, "0.0.0.0") == 0)
    {
        address.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, ip_address, &address.sin_addr) <= 0)
        {
            perror("Invalid IP address");
            exit(EXIT_FAILURE);
        }
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Pthread server listening on %s:%d...\n", ip_address, port);
    printf("Press Ctrl+C to stop the server\n");

    while (1)
    {

        client_socket_ptr = malloc(sizeof(int));

        if ((*client_socket_ptr = accept(server_fd, (struct sockaddr *)&address,
                                         (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            free(client_socket_ptr);
            continue;
        }

        if (pthread_create(&thread_id, NULL, handle_client_thread, (void *)client_socket_ptr) != 0)
        {
            perror("pthread_create failed");
            close(*client_socket_ptr);
            free(client_socket_ptr);
        }
        else
        {

            pthread_detach(thread_id);
        }
    }
}

void display_menu()
{
    printf("\n=== Concurrent Server Selection Menu ===\n");
    printf("1. Fork-based Server\n");
    printf("2. Pthread-based Server\n");
    printf("3. Exit\n");
    printf("Please select an option (1-3): ");
}

int main()
{
    int choice;
    char ip_address[16];
    int port;

    printf("=== Concurrent Server Setup ===\n");

    printf("Enter IP address to bind (0.0.0.0 for all interfaces): ");
    scanf("%15s", ip_address);

    printf("Enter port number (default 8080): ");
    if (scanf("%d", &port) != 1 || port <= 0 || port > 65535)
    {
        port = DEFAULT_PORT;
        printf("Invalid port. Using default port: %d\n", DEFAULT_PORT);
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    while (1)
    {
        display_menu();

        if (scanf("%d", &choice) != 1)
        {

            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input. Please enter a number (1-3).\n");
            continue;
        }

        while ((c = getchar()) != '\n' && c != EOF)
            ;

        switch (choice)
        {
        case 1:
            printf("\nStarting Fork-based Server...\n");
            fork_based_server(ip_address, port);
            break;

        case 2:
            printf("\nStarting Pthread-based Server...\n");
            pthread_based_server(ip_address, port);
            break;

        case 3:
            printf("Exiting program. Goodbye!\n");
            exit(EXIT_SUCCESS);

        default:
            printf("Invalid choice! Please select 1, 2, or 3.\n");
        }
    }

    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P34$ gcc 34s.c -o 34s

2. Run
Fork-based Server :
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P34$ ./34s
=== Concurrent Server Setup ===
Enter IP address to bind (0.0.0.0 for all interfaces): 0.0.0.0
Enter port number (default 8080): 8080

-------------------------------- Fork-based Server : --------------------------------
=== Concurrent Server Selection Menu ===
1. Fork-based Server
2. Pthread-based Server
3. Exit
Please select an option (1-3): 1

Starting Fork-based Server...
Starting fork-based concurrent server on 0.0.0.0:8080...
Fork server listening on 0.0.0.0:8080...
Press Ctrl+C to stop the server
Client connected: 127.0.0.1:40510
Received from 127.0.0.1:40510: This is Fork-based Server


-------------------------------- Pthread-based Server : --------------------------------
=== Concurrent Server Selection Menu ===
1. Fork-based Server
2. Pthread-based Server
3. Exit
Please select an option (1-3): 2

Starting Pthread-based Server...
Starting pthread-based concurrent server on 127.0.0.1:8080...
Pthread server listening on 127.0.0.1:8080...
Press Ctrl+C to stop the server
Client connected: 127.0.0.1:46018
Received from 127.0.0.1:46018: This is Pthread-based Server

============================================================================
*/
