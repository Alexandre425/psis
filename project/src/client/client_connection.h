#pragma once

// Defines functions that handle the connection between the client and the server

// Connects to the game server, returns it's socket
int connect_to_server (char* server_ip, char* server_port);
