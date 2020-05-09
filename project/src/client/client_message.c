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
    MessageType mt = htons(message_color);
    send_all(socket, &mt, sizeof(MessageType));

    // Send the message itself
    uint32_t net_color = htonl(color);
    send_all(socket, &net_color, sizeof(uint32_t));

    // Send the terminator to confirm end of message
    mt = message_terminator;
    send_all(socket, &mt, sizeof(MessageType));
}