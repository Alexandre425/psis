#pragma once

#include "../common/utilities.h"
#include "server.h"

// Implements functions and structures regarding how messages are sent and received from the client

// Receives the player's color from a client
void message_recv_color(int socket, Color* color);

// Sends game the board to a client
void message_send_board(int socket, Board* board);

// Sends the list of players to a client
void message_send_player_list(int socket, Game* game);
