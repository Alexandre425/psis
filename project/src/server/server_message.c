#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../common/pacman.h"
#include "../common/message.h"
#include "../common/utilities.h"
#include "server_message.h"

void message_recv_color(int socket, Color* color)
{
    // Receive the data
    message_recv_uint32_t(socket, (uint32_t*)color);
}
