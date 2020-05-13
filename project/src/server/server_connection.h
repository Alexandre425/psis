#pragma once

#include "../common/message.h"
#include "server.h"

// Defines functions that handle the connections between the server and the client

// Opens a socket, waits for a client to connect and opens a thread for that client
// Opens a new socket every time a client connects, unless the maximum number of clients has been reached
void* connect_to_clients (void* game);

// Is in charge of receiving information from a single client
// Receives a Client struct cast as void*
// Threaded from connect_to_clients()
void* recv_from_client (void* client);

// Sends a message of type message_type to all the clients currently connected
// All clients receive the same message
// The message's contents are taken from the game struct
void send_to_all_clients(Game* game, MessageType message_type);
