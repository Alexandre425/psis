#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>

#include "client_connection.h"

void connect_to_server (char* server_ip, char* server_port)
{
    puts("Connecting to server");

    // Create an unnamed socket for the server
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("ERROR - Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server's address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    short port;
    sscanf(server_port, "%hi", &port);
    server_addr.sin_port = htons(port);
    // Verify if the IP is valid
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("ERROR - Introduced invalid IP");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    
    {
        perror("ERROR - Server connection error");
        exit(EXIT_FAILURE);
    }

    puts("Connection established");
}