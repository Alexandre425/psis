#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "utilities.h"

void color_hex_to_rgb(Color color, unsigned int* r, unsigned int* g, unsigned int* b)
{
    // Bitwise AND with the respective byte, then shift it right to make it the LSB
    *b = (color & 0x0000FF) >> 0;
    *g = (color & 0x00FF00) >> 8;
    *r = (color & 0xFF0000) >> 16;
}


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

int abs_int(int value)
{
    if (value >= 0)
        return value;
    else
        return -value;
}

int time_diff_ms(struct timespec then, struct timespec now)
{
    int time_diff = 1000 * (now.tv_sec - then.tv_sec);
    time_diff += (now.tv_nsec - then.tv_nsec) / 1000000;
    return time_diff;
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
            return -1;
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

void change_to_exec_dir(char* argv0)
{
    char* c = argv0, *last_slash = argv0;
    while (*c != '\0')          // While the end of the string hasn't been reached
    {
        if (*c == '/')          // Register the location of a forward slash if found
            last_slash = c;
        c++;
    }
    *(++last_slash) = '\0';     // Terminate the string at the last slash
    chdir(argv0);               // Change directory to the resulting string
}

int min (int a, int b)
{
    return a < b ? a : b;
}
int max (int a, int b)
{
    return a > b ? a : b;
}