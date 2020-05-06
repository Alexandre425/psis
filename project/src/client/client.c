#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>

#include "../../3rd/UI_library.h"
#include "client_connection.h"

int main (int argc, char* argv[])
{
    // Verify correct argument usage
    if (argc != 2)
    {
        puts("Usage: ./client <ip>:<port>");
        exit(EXIT_FAILURE);
    }
    // Read server IP and port from string
    char* ip_str = strtok(argv[1], ":");
    char* port_str = strtok(NULL, ":");

    connect_to_server(ip_str, port_str);

    create_board_window(10, 10);

    return 0;
}