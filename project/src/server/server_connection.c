#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "server_connection.h"

#define PLACEHOLDER_PORT 25000

void* connect_to_clients (void* a)
{
    // Create an unnamed INET socket
    int listen_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        perror("ERROR - Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a name for the socket
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(PLACEHOLDER_PORT);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Address to accept any incoming message

    // Bind socket to name
    bind(listen_socket, (const struct sockaddr*)&listen_addr, sizeof(listen_addr));

    // Tell the kernel to listen on this socket
    listen(listen_socket, 5);

    // Accept all incoming connections
    int client_socket;
    while (1)
    {
        puts("Waiting for client");
        // Accept blocks until a client connects
        client_socket = accept(listen_socket, NULL, NULL);   // Client address is ignored
        if (client_socket == -1)
        {
            perror("ERROR - Accept failed");
            exit(EXIT_FAILURE);
        }

        // Thread to handle client communications
    }
    
}