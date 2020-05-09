#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

#include "server_connection.h"
#include "server_message.h"
#include "server.h"
#include "../common/message.h"
#include "../common/utilities.h"

#define PLACEHOLDER_PORT 25000

typedef struct _client
{
    unsigned int player_id;
    int socket;
} Client;

void* connect_to_clients (void* game)
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
        // Accept connections - blocks until a client connects
        client_socket = accept(listen_socket, NULL, NULL);   // Client address is ignored
        if (client_socket == -1)
        {
            perror("ERROR - Accept failed");
            exit(EXIT_FAILURE);
        }

        // Create a new client struct to pass to the receiver function
        Client* client = malloc_check(sizeof(Client));
        client->socket = client_socket;

        // Create a new player
        client->player_id = player_create(game);

        fprintf(stdout, "Player %d connected!\n", client->player_id);

        // Thread to handle client communications
        pthread_t recv_from_client_thread;
        pthread_create(&recv_from_client_thread, NULL, recv_from_client, (void*)client);
    }
}

void* recv_from_client (void* _client)
{
    // Cast to Client
    Client* client = (Client*)_client;
    
    // Probe for new messages repeatedly
    while (1)
    {
        // Determine message type
        MessageType mt;
        if (recv_all(client->socket, &mt, sizeof(MessageType)) == 0)
        {
            puts("Client left");
            break;
        }
        // Use function respective to type
        switch (ntohs(mt))
        {
        case message_color:
        {
            Color color;
            message_recv_color(client->socket, &color);
            printf("Received color %x from player %d\n", color, client->player_id);
            break;
        }
        
        default:
            break;
        }
    }

    return NULL;
}
