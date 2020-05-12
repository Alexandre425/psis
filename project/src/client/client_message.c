#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../common/utilities.h"
#include "../common/message.h"
#include "client_message.h"

// Good example on how most of the sends work:
void message_send_color(int socket, Color color)
{
    // Send the message type so the receiver can interpret it correctly
    message_send_uint16_t(socket, (uint16_t)MESSAGE_COLOR);

    // Send the message itself
    message_send_uint32_t(socket, (uint32_t)color);

    // Send the terminator to confirm end of message
    message_send_uint16_t(socket, (uint16_t)MESSAGE_TERMINATOR);
}