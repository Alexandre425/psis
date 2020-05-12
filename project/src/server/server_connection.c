#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#include "server_connection.h"
#include "server_message.h"
#include "server.h"
#include "../common/message.h"
#include "../common/utilities.h"

const uint16_t PORT = 25000;

typedef struct _client
{
    Game* game;
    unsigned int player_id;
    int socket;
} Client;

static unsigned int n_clients = 0;
static pthread_t* client_array = NULL;

// Stores the new client's thread handler in the array
static void client_store(pthread_t client_thread)
{
    // If first client
    if (!client_array)
    {
        n_clients++;
        client_array = malloc_check(sizeof(pthread_t));
    }
    else
    {
        n_clients++;
        client_array = realloc_check(client_array, n_clients * sizeof(pthread_t));
    }
    client_array[n_clients - 1] = client_thread;
}

// Removes a client's thread handler from the array
static void client_destroy(pthread_t client_thread)
{
    for (unsigned int i = 0; i < n_clients; ++i)
    {
        if (client_thread == client_array[i])
        {
            // Overwrite with last
            client_array[i] = client_array[n_clients - 1];
            n_clients--;
            return;
        }
    }
    puts("ERROR - Attempted to destroy a missing client");
    exit(EXIT_FAILURE);
}

// Signal handler for the connect_to_clients thread
// Interrupts the blocking accept()
void shutdown_handler(int unused)
{
    for (unsigned int i = 0; i < n_clients; ++i)
    {
        pthread_kill(client_array[i], SIGUSR1);
    }
}
// Signal handler for the recv_from_client threads
// Interrupts the blocking accept()
static void null_handler(int unused)
{
    // Do nothing
}

void* connect_to_clients (void* game)
{
    // Handle the shutdown signal
    struct sigaction action;
    action.sa_handler = shutdown_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        perror("ERROR - Could not assign signal handler");
        exit(EXIT_FAILURE);
    }

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
    listen_addr.sin_port = htons(PORT);
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
        client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == -1)
        {
            // If accept was interrupted by a signal (means server is shutting down)
            if (errno == EINTR)
            {
                break;
            }
            else
            {
                perror("ERROR - Accept failed");
                exit(EXIT_FAILURE);
            }
        }

        // Create a new client struct to pass to the receiver function
        Client* client = malloc_check(sizeof(Client));
        client->socket = client_socket;
        client->game = (Game*)game;

        // Create a new player
        client->player_id = player_create((Game*)game);

        fprintf(stdout, "Player %d connected!\n", client->player_id);

        // Thread to handle client communications
        pthread_t recv_from_client_thread;
        pthread_create(&recv_from_client_thread, NULL, recv_from_client, (void*)client);
        client_store(recv_from_client_thread);
    }

    puts("close");
    // Close the listening socket
    shutdown(listen_socket, SHUT_RDWR);
    return NULL;
}

void* recv_from_client (void* _client)
{
    // Cast to Client
    Client* client = (Client*)_client;

    Player* player = player_find_by_id(client->game, client->player_id);

    // Handle the shutdown signal
    struct sigaction action;
    action.sa_handler = null_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        perror("ERROR - Could not assign signal handler");
        exit(EXIT_FAILURE);
    }
    
    // Probe for new messages repeatedly
    while (1)
    {
        // Determine message type
        MessageType mt;
        int ret = recv_all(client->socket, &mt, sizeof(MessageType));
        if (ret == 0)
        {
            fprintf(stdout, "Client %d left!\n", client->player_id);
            break;
        }
        else if (ret == -1)
        {
            if (errno == EINTR)
            {
                break;
            }
            else
            {
                perror("ERROR - Could not receive data");
                exit(EXIT_FAILURE);
            }
        }
        // Use function respective to type
        switch (ntohs(mt))
        {
        case MESSAGE_COLOR:
        {
            Color color;
            message_recv_color(client->socket, &color);
            player_set_color(player, color);
            printf("Received color %x from player %d\n", color, client->player_id);
            break;
        }
        
        default:
            break;
        }

        // Receive the terminator
        message_recv_uint16_t(client->socket, (uint16_t*)&mt);
        if (mt != MESSAGE_TERMINATOR)
            message_misaligned();
    }
    
    // Destroy the player who left
    player_destroy(client->game, client->player_id);
    // Remove self from the client handler array
    client_destroy(pthread_self());
    // Terminate connection with client
    shutdown(client->socket, SHUT_RDWR);
    free(client);

    return NULL;
}
