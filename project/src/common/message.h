#pragma once

#include <stdlib.h>

#include "utilities.h"
#include "pacman.h"

// Implements functions that serialize data to be sent over the network

// Precedes every message, tells the server/client how to interpret the following bytes
typedef uint16_t MessageType;
enum _MessageType {MESSAGE_TERMINATOR = UINT16_MAX, MESSAGE_COLOR = 0, MESSAGE_BOARD};

// Ran when a misaligned message is detected in the receiving end
void message_misaligned(void);

// Sends a 2 byte unsigned integer
// Handles endianess and errors
void message_send_uint16_t(int socket, uint16_t message);
// Sends a 2 byte signed integer
// Handles endianess and errors
void message_send_int16_t(int socket, int16_t message);
// Sends a 4 byte unsigned integer
// Handles endianess and errors
void message_send_uint32_t(int socket, uint32_t message);
// Sends a 4 byte signed integer
// Handles endianess and errors
void message_send_int32_t(int socket, int32_t message);

// Receives a 2 byte unsigned integer, puts it in message
// Handles endianess and errors
// Returns 1 on success, 0 on socket close
int message_recv_uint16_t(int socket, uint16_t* message);
// Receives a 2 byte signed integer, puts it in message
// Handles endianess and errors
// Returns 1 on success, 0 on socket close
int message_recv_int16_t(int socket, int16_t* message);
// Receives a 4 byte unsigned integer, puts it in message
// Handles endianess and errors
// Returns 1 on success, 0 on socket close
int message_recv_uint32_t(int socket, uint32_t* message);
// Receives a 4 byte signed integer, puts it in message
// Handles endianess and errors
// Returns 1 on success, 0 on socket close
int message_recv_int32_t(int socket, int32_t* message);