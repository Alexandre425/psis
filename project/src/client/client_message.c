#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../common/utilities.h"
#include "../common/message.h"
#include "client_message.h"

// Good example on how most of the sends work:
void message_send_color(int socket, Color color)
{
    // Send the message type so the receiver can interpret it correctly
    message_send_uint16_t(socket, (uint16_t)MESSAGE_COLOR);

    // Send the message itself
    message_send_uint32_t(socket, (uint32_t)color);

    // Send the terminator to confirm end of message
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_movement_pac(int socket, char movement_dir)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_MOVE_PAC);
    message_send_char(socket, (char)movement_dir);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_movement_mon(int socket, char movement_dir)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_MOVE_MON);
    message_send_char(socket, (char)movement_dir);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_recv_player_id(int socket, unsigned int* player_id)
{
    message_recv_uint32_t(socket, (uint32_t*)player_id);
}

void message_recv_board(int socket, Board** board)
{
    // Receive the size (x then y)
    uint16_t size_x, size_y;
    message_recv_uint16_t(socket, (uint16_t*)&size_x);
    message_recv_uint16_t(socket, (uint16_t*)&size_y);
    // If the board doesn't exist yet
    if (!*board)
        *board = board_create(size_x, size_y);
    // Go line by line and receive the board's tiles sequentially, storing them
    uint16_t tile;
    for (unsigned int i = 0; i < size_y; ++i)
    {
        for (unsigned int j = 0; j < size_x; ++j)
        {
            message_recv_uint16_t(socket, (uint16_t*)&tile);
            board_set_tile(*board, j, i, (unsigned int)tile);
        }
    }
}

void message_recv_player_list(int socket, Game* game)
{
    // Receive the number of players
    unsigned int n_players;
    message_recv_uint16_t(socket, (uint16_t*)&n_players);

    // For every player
    for (unsigned int i = 0; i < n_players; ++i)
    {
        uint16_t player_id;
        message_recv_uint16_t(socket, (uint16_t*)&player_id);   // Receive the player ID
        Player* player = player_find_by_id(game, (unsigned int)player_id);
        // If the player doesn't exists in the client's memory
        if (!player)
        {
            player = player_create(game, player_id);
        }
        Color color;                                            // Receive the color
        message_recv_uint32_t(socket, (uint32_t*)&color);
        player_set_color(player, color);
        int x, y;                                               // The pacman and monster positions
        message_recv_uint32_t(socket, (uint32_t*)&x);
        message_recv_uint32_t(socket, (uint32_t*)&y);
        player_set_pac_pos(player, x, y);
        message_recv_uint32_t(socket, (uint32_t*)&x);
        message_recv_uint32_t(socket, (uint32_t*)&y);
        player_set_mon_pos(player, x, y);   
        bool powered_up;                                        // The power up state
        message_recv_char(socket, (char*)&powered_up);
        player_set_power_up_state(player, powered_up);
    }
}

void message_recv_player_disconnect(int socket, unsigned int* player_id)
{
    message_recv_uint32_t(socket, (uint32_t*)player_id);
}


void message_send_player_disconnect(int socket, unsigned int player_id)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_PLAYER_DISCONNECT);
    message_send_uint32_t(socket, (uint32_t)player_id);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}