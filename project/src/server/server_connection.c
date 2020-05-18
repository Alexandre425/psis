#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#include "server_connection.h"
#include "server_message.h"
#include "../common/utilities.h"

pthread_mutex_t client_array_lock;
const uint16_t PORT = 25000;

typedef struct _client
{
    Game* game;
    pthread_t handler_thread;
    unsigned int player_id;
    int socket;
} Client;

static unsigned int n_clients = 0;
static Client** client_array = NULL;

// Stores the new client struct's pointer in the array
static void client_store(Client* client)
{
    pthread_mutex_lock(&client_array_lock);
    // If first client
    if (!client_array)
    {
        n_clients++;
        client_array = malloc_check(sizeof(Client*));
    }
    else
    {
        n_clients++;
        client_array = realloc_check(client_array, n_clients * sizeof(Client*));
    }
    client_array[n_clients - 1] = client;
    pthread_mutex_unlock(&client_array_lock);

}

// Removes a client's pointer from the array
static void client_destroy(Client* client)
{
    pthread_mutex_lock(&client_array_lock);
    for (unsigned int i = 0; i < n_clients; ++i)
    {
        // Compare the pointers
        if (client == client_array[i])
        {
            // Overwrite with last
            client_array[i] = client_array[n_clients - 1];
            n_clients--;
            pthread_mutex_unlock(&client_array_lock);
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
        pthread_kill(client_array[i]->handler_thread, SIGUSR1);
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

    // Create the mutex for the client array
    if (pthread_mutex_init(&client_array_lock, NULL))
    {
        perror("ERROR - Mutex init failed");
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

        // Send the client their player ID
        message_send_player_id(client->socket, client->player_id);

        // Thread to handle client communications
        pthread_t recv_from_client_thread;
        pthread_create(&recv_from_client_thread, NULL, recv_from_client, (void*)client);
        client->handler_thread = recv_from_client_thread;
        client_store(client);
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
        int ret = message_recv_uint16_t(client->socket, (uint16_t*)&mt);
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
        switch (mt)
        {
        case MESSAGE_COLOR:
        {
            Color color;
            message_recv_color(client->socket, &color);
            player_set_color(player, color);
            break;
        }
        case MESSAGE_MOVE_PAC:
        {
            char move_dir;
            message_recv_movement(client->socket, (char*)&move_dir);
            player_set_pac_move_dir(player, move_dir);
            printf("Received movement dir %c from player %d\n", move_dir, client->player_id);
            break;
        }
        case MESSAGE_MOVE_MON:
        {
            char move_dir;
            message_recv_movement(client->socket, (char*)&move_dir);
            player_set_mon_move_dir(player, move_dir);
            printf("Received movement dir %c from player %d\n", move_dir, client->player_id);
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
    client_destroy(client);
    // Terminate connection with client
    shutdown(client->socket, SHUT_RDWR);
    free(client);

    return NULL;
}

void send_to_all_clients(Game* game, MessageType message_type)
{
    pthread_mutex_lock(&client_array_lock);
    // For every client
    for (unsigned int i = 0; i < n_clients; ++i)
    {
        // Send the message defined by message_type
        switch (message_type)
        {
        case MESSAGE_BOARD:
            message_send_board(client_array[i]->socket, game_get_board(game));
            break;
        case MESSAGE_PLAYER_LIST:
            message_send_player_list(client_array[i]->socket, game);
            break;

        default:
            break;
        }

    }
    pthread_mutex_unlock(&client_array_lock);
}