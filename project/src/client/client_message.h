#pragma once

#include "../common/utilities.h"
#include "../common/pacman.h"
#include "client.h"

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
// Creates a board if *board is NULL
// Stores the pointer to the new board in board
void message_recv_board(int socket, Board** board);
// Receives the list of players from the server
// If an incoming player isn't in the client's memory, this function creates them
void message_recv_player_list(int socket, Game* game);
// Receives a player ID, specifically the client's own ID
void message_recv_player_id(int socket, unsigned int* player_id);
// Receives a player ID corresponding to a disconnected player
void message_recv_player_disconnect(int socket, unsigned int* player_id);

// Receives the list of fruit
void message_recv_fruit_list(int socket, Game* game);


