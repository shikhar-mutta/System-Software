/*
============================================================================
Name : 34c.c
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

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed\n");
        return -1;
    }

    printf("Connected to server. Type 'exit' to quit.\n");

    while (1)
    {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        send(sock, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }

        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            buffer[valread] = '\0';
            printf("Server echo: %s", buffer);
        }
    }

    close(sock);
    return 0;
}

/*
============================================================================
Compilation :

1. Compile
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P34$ gcc 34c.c -o 34c

2. Run

-------------------------------- Fork-based Server : --------------------------------
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P34$ ./34c
Connected to server. Type 'exit' to quit.
Enter message: This is Fork-based Server
Server echo: This is Fork-based Server

-------------------------------- Pthread-based Server : --------------------------------
shikhar@shikhar-pc:~/Sem 1/Softeware Sysyem /Hands-on list 2/P34$ ./34c
Connected to server. Type 'exit' to quit.
Enter message: This is Pthread-based Server
Server echo: This is Pthread-based Server

============================================================================
*/
