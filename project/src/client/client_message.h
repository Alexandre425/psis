#pragma once

#include "../common/utilities.h"

// Implements functions and structures regarding how messages are sent and received from the server

// Sends the player's color to the server
void message_send_color(int socket, Color color);

// Receives the game board from the server
// Stores the pointer to the board in board
void message_recv_board(int socket, Board** board);
