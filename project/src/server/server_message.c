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
    // Receive the data
    message_recv_uint32_t(socket, (uint32_t*)color);
}

void message_send_board(int socket, Board* board)
{
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
}