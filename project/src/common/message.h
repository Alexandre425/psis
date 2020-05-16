#pragma once

#include <stdlib.h>

#include "utilities.h"
#include "pacman.h"

// Implements functions that serialize data to be sent over the network

// Precedes every message, tells the server/client how to interpret the following bytes
// The send functions should send the message type, along with the terminator at the end
// The type and terminator are not handled by the recv functions, and must be handled externally to allow correct selection of a recv function
typedef uint16_t MessageType;
enum _MessageType {
    // Terminates a message, used for checking allignment
    MESSAGE_TERMINATOR = UINT16_MAX, 
    // Player color
    MESSAGE_COLOR = 0, 
    // The game board
    MESSAGE_BOARD,
    // The movement intent of the client's Pacman
    MESSAGE_MOVE_PAC, 
    // The movement intent of the client's monster
    MESSAGE_MOVE_MON, 
    // The ID of the client
    MESSAGE_PLAYER_ID,
    // The list of players
    MESSAGE_PLAYER_LIST};

// Ran when a misaligned message is detected in the receiving end
void message_misaligned(void);

// Sends a 1 byte char
// Handles endianess
// Returns 1 on success, -1 on error
int message_send_char(int socket, char message);
// Sends a 2 byte unsigned integer
// Handles endianess
// Returns 1 on success, -1 on error
int message_send_uint16_t(int socket, uint16_t message);
// Sends a 2 byte signed integer
// Handles endianess
// Returns 1 on success, -1 on error
int message_send_int16_t(int socket, int16_t message);
// Sends a 4 byte unsigned integer
// Handles endianess
// Returns 1 on success, -1 on error
int message_send_uint32_t(int socket, uint32_t message);
// Sends a 4 byte signed integer
// Handles endianess
// Returns 1 on success, -1 on error
int message_send_int32_t(int socket, int32_t message);

// Receives a 1 byte char, puts it in message
// Handles endianess
// Returns 1 on success, 0 on socket close, -1 on error
int message_recv_char(int socket, char* message);
// Receives a 2 byte unsigned integer, puts it in message
// Handles endianess
// Returns 1 on success, 0 on socket close, -1 on error
int message_recv_uint16_t(int socket, uint16_t* message);
// Receives a 2 byte signed integer, puts it in message
// Handles endianess
// Returns 1 on success, 0 on socket close, -1 on error
int message_recv_int16_t(int socket, int16_t* message);
// Receives a 4 byte unsigned integer, puts it in message
// Handles endianess
// Returns 1 on success, 0 on socket close, -1 on error
int message_recv_uint32_t(int socket, uint32_t* message);
// Receives a 4 byte signed integer, puts it in message
// Handles endianess
// Returns 1 on success, 0 on socket close, -1 on error
int message_recv_int32_t(int socket, int32_t* message);