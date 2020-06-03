#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>

#include "client.h"
#include "client_connection.h"
#include "client_message.h"

#include "../common/message.h"

int connect_to_server (char* server_ip, char* server_port)
{
    puts("Connecting to server");

    // Create an unnamed socket for the server
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("ERROR - Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server's address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    short port;
    sscanf(server_port, "%hi", &port);
    server_addr.sin_port = htons(port);
    // Verify if the IP is valid
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("ERROR - Introduced invalid IP");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if(connect(server_socket, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("ERROR - Server connection error");
        exit(EXIT_FAILURE);
    }

    puts("Connection established!");

    return server_socket;
}

void* recv_from_server(void* _game)
{
    // Cast to game
    Game* game = (Game*)_game;
    int server_socket = game_get_server_socket(game);

    // Probe for new messages
    while (1)
    {
        // Determine message type
        MessageType mt;
        int ret = message_recv_uint16_t(server_socket, (uint16_t*)&mt);
        if (ret == 0)
        {
            puts("Server disconnected!");
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
        switch (mt)
        {
        case MESSAGE_BOARD:
        {
            Board* board;
            message_recv_board(server_socket, &board);
            game_set_board(game, board);
            break;
        }
        case MESSAGE_PLAYER_LIST:
        {
            message_recv_player_list(server_socket, game);
            break;
        }
        case MESSAGE_PLAYER_ID:
        {
            unsigned int player_id;
            message_recv_player_id(server_socket, &player_id);
            game_set_player_id(game, player_id);
            break;
        }
        case MESSAGE_SERVER_FULL:
        {
            message_recv_server_full(server_socket);
            break;
        }

        case MESSAGE_PLAYER_DISCONNECT:
        {
            unsigned int player_id;
            message_recv_player_disconnect(server_socket, &player_id);
            player_destroy(game, player_id);
            break;
        }
        case MESSAGE_FRUIT_LIST:
        {
            message_recv_fruit_list(server_socket, game);
            break;
        }
        case MESSAGE_PRINT_SCOREBOARD:
            message_recv_print_scoreboard_order(server_socket, game);
            break;
        
        default:
            break;
        }

        // Receive the terminator
        message_recv_uint16_t(server_socket, (uint16_t*)&mt);
        if (mt != MESSAGE_TERMINATOR)
            message_misaligned();
    }
    client_quit();
    return NULL;
}