#pragma once

#include "../common/utilities.h"

// Implements functions and structures regarding how messages are sent and received from the server

// Sends the player's color to the server
void message_send_color(int socket, Color color);

// Sends the intended movement of the Pacman to the server
// movement_dir should be w, a, s or d for up, left, down or right, respectively
// If movement_dir is 0, the character is to stand still
void message_send_movement_pac(int socket, char movement_dir);

// Sends the intended movement of the monster to the server
// movement_dir should be w, a, s or d for up, left, down or right, respectively
// If movement_dir is 0, the character is to stand still
void message_send_movement_mon(int socket, char movement_dir);

// Receives the game board from the server
// Stores the pointer to the board in board
void message_recv_board(int socket, Board** board);
