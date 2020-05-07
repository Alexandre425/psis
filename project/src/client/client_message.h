#pragma once

#include "../common/utilities.h"

// Implements functions and structures regarding how messages are sent and received from the server

// Sends the player's color to the server
void message_send_color(int socket, Color color);
