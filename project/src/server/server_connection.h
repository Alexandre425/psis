#pragma once

// Defines functions that handle the connections between the server and the client

// Opens a socket, waits for a client to connect and opens a thread for that client
// Opens a new socket every time a client connects, unless the maximum number of clients has been reached
void* connect_to_clients (void* game);

// Is in charge of receiving information from a single client
// Threaded from connect_to_clients()
void* recv_from_client (void* game);