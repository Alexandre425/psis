#pragma once

#include "../common/utilities.h"
#include "server.h"

// Implements functions and structures regarding how messages are sent and received from the client

// Receives the player's color from a client
void message_recv_color(int socket, Color* color);
// Receives the player's intended movement direction (for the monster OR the pacman, depends on the MessageType) from a client
void message_recv_movement(int socket, char* movement_dir);

// Sends a client their own player ID
void message_send_player_id(int socket, unsigned int player_id);
// Sends game the board to a client
void message_send_board(int socket, Board* board);
// Sends the list of players to a client
void message_send_player_list(int socket, Game* game);
// Sends the player ID of a disconnecting client to a client
void message_send_player_disconnect(int socket, unsigned int player_id);
// Sends the list of fruit
void message_send_fruit_list(int socket, Game* game);
// Sends the order to print the scoreboard clientside
void message_send_print_scoreboard_order(int socket);
// Sends an alert for a full server
void message_send_server_full(int socket);
