#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "../common/pacman.h"
#include "../common/utilities.h"
#include "server_message.h"

void message_recv_color(int socket, Color* color)
{
    recv_all(socket, color, sizeof(Color));
}
