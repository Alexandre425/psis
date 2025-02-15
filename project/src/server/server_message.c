#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../common/pacman.h"
#include "../common/message.h"
#include "../common/utilities.h"
#include "server_message.h"

void message_recv_color(int socket, Color* color)
{
    message_recv_uint32_t(socket, (uint32_t*)color);
}

void message_recv_movement(int socket, char* movement_dir)
{
    message_recv_char(socket, (char*)movement_dir);
}

void message_send_player_id(int socket, unsigned int player_id)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_PLAYER_ID);
    message_send_uint32_t(socket, (uint32_t)player_id);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}


void message_send_board(int socket, Board* board)
{
    // Send the message type
    message_send_uint16_t(socket, (uint16_t)MESSAGE_BOARD);
    // Send the size (x then y)
    message_send_uint16_t(socket, (uint16_t)board_get_size_x(board));
    message_send_uint16_t(socket, (uint16_t)board_get_size_y(board));
    // Go line by line and send the board's tiles sequentially
    for (unsigned int i = 0; i < board_get_size_y(board); ++i)
    {
        for (unsigned int j = 0; j < board_get_size_x(board); ++j)
        {
            message_send_uint16_t(socket, (uint16_t)board_get_tile(board, j, i));
        }
    }
    // Send the terminator
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);

}

void message_send_player_list(int socket, Game* game)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_PLAYER_LIST);

    unsigned int n_players = 0;
    Player** players = game_get_player_array(game, &n_players);
    
    // Send the number of players
    message_send_uint16_t(socket, (uint16_t)n_players);
    // For every player
    for (unsigned int i = 0; i < n_players; ++i)
    {
        Player* player = players[i];
        message_send_uint16_t(socket, (uint16_t)player_get_id(player));             // Send the player id
        message_send_uint32_t(socket, (uint32_t)player_get_score(player));          // Send the score
        message_send_uint32_t(socket, (uint32_t)player_get_color(player));          // Send the color
        message_send_int32_t(socket, (int32_t)player_get_pac_pos_x(player));        // Send the pacman's position (x then y)
        message_send_int32_t(socket, (int32_t)player_get_pac_pos_y(player));
        message_send_int32_t(socket, (int32_t)player_get_mon_pos_x(player));        // Send the monster's position (x then y)
        message_send_int32_t(socket, (int32_t)player_get_mon_pos_y(player));
        message_send_int32_t(socket, (int32_t)player_get_power_up_state(player));   // Send the power up state
    }

    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_fruit_list(int socket, Game* game)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_FRUIT_LIST);

    unsigned int n_fruits;
    Fruit** fruits = game_get_fruit_array(game, &n_fruits);

    message_send_uint16_t(socket, (uint16_t)n_fruits);
    for (unsigned int i = 0; i < n_fruits; ++i)
    {
        Fruit* fruit = fruits[i];
        message_send_uint32_t(socket, (uint32_t)fruit_get_type(fruit));     // Send the type
        message_send_int32_t(socket, (int32_t)fruit_get_pos_x(fruit));      // Send the position
        message_send_int32_t(socket, (int32_t)fruit_get_pos_y(fruit));
        message_send_int32_t(socket, (int32_t)fruit_get_is_alive(fruit));   // Send wether the fruit is alive or not
    }
    
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_player_disconnect(int socket, unsigned int player_id)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_PLAYER_DISCONNECT);
    message_send_uint32_t(socket, (uint32_t)player_id);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_print_scoreboard_order(int socket)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_PRINT_SCOREBOARD);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}

void message_send_server_full(int socket)
{
    message_send_uint16_t(socket, (uint16_t)MESSAGE_SERVER_FULL);
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}