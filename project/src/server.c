#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#include "UI_library.h"
#include "server_connection.h"

int main (void)
{
    // Open board file

    // Calculate maximum number of clients

    // Etc

    // Thread for handling incoming client connections
    pthread_t connect_to_clients_thread;
    pthread_create(&connect_to_clients_thread, NULL, connect_to_clients, NULL);

    pthread_join(connect_to_clients_thread, NULL);

    return 0;
}