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
    uint32_t net_color;
    recv_all(socket, &net_color, sizeof(Color));
    *color = ntohl(net_color);

    // Receive the terminator
    MessageType mt;
    recv_all(socket, &mt, sizeof(Color));
    if (ntohs(mt) != message_terminator)
        message_misaligned();

}
