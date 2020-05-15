#include <stdio.h>
#include <stdlib.h>

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


void message_recv_board(int socket, Board** board)
{
    // Receive the size (x then y)
    uint16_t size_x, size_y;
    message_recv_uint16_t(socket, (uint16_t*)&size_x);
    message_recv_uint16_t(socket, (uint16_t*)&size_y);
    // Create the board
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