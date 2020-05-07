#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>

#include "../common/UI_library.h"
#include "../common/pacman.h"
#include "client_connection.h"
#include "client_message.h"

int main (int argc, char* argv[])
{
    // Verify correct argument usage
    if (argc != 3)
    {
        puts("Usage: ./client <ip>:<port> <color (hex RRGGBB format)>");
        exit(EXIT_FAILURE);
    }
    // Read server IP and port from string
    char* ip_str = strtok(argv[1], ":");
    char* port_str = strtok(NULL, ":");
    // Read the color
    Color color;
    sscanf(argv[2], "%x", &color);
    printf("%d %x", color, color);

    int server_socket = connect_to_server(ip_str, port_str);


    return 0;
}