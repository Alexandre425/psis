#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "../common/utilities.h"
#include "client_message.h"

void message_send_color(int socket, Color color)
{
    send_all(socket, &color, sizeof(Color));
}