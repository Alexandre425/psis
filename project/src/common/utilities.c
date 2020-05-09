#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "utilities.h"

void* malloc_check(size_t size)
{
    void* ptr = calloc(1, size);
    if (ptr == NULL)
    {
        perror("ERROR - Could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void* realloc_check(void* ptr, size_t size)
{
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL)
    {
        perror("ERROR - Could not allocate memory");
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}


int send_all (int socket, void* buffer, size_t size)
{
    char* ptr = (char*)buffer;
    // While there is data to be sent
    while (size > 0)
    {
        size_t sent = send(socket, ptr, size, 0);
        if (sent == -1)
        {
            perror("ERROR - Data send failed");
        }
        // Update the data still to be sent
        ptr += sent;
        size -= sent;
    }
    return 1;
}

int recv_all (int socket, void* buffer, size_t size)
{
    char* ptr = (char*)buffer;
    // While there is data to be received
    while (size > 0)
    {
        ssize_t recvd = recv(socket, buffer, size, 0);
        // Handles both the socket close and error
        if (recvd < 1)
            return recvd;
        ptr += recvd;
        size -= recvd;
    }
    return 1;
}
