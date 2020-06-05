#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "message.h"

void message_misaligned(void)
{
    puts("ERROR - Message misalignment detected");
    exit(EXIT_FAILURE);
}

// Data sends
int message_send_char(int socket, char message)
{
    char m_net = message;
    return send_all(socket, (void*)&m_net, sizeof(char));
}

int message_send_uint16_t(int socket, uint16_t message)
{
    uint16_t m_net = htons(message);
    return send_all(socket, (void*)&m_net, sizeof(uint16_t));
}

int message_send_int16_t(int socket, int16_t message)
{
    int16_t m_net = htons(message);
    return send_all(socket, (void*)&m_net, sizeof(int16_t));
}

int message_send_uint32_t(int socket, uint32_t message)
{
    uint32_t m_net = htonl(message);
    return send_all(socket, (void*)&m_net, sizeof(uint32_t));
}

int message_send_int32_t(int socket, int32_t message)
{
    uint32_t m_net = htonl(message);
    return send_all(socket, (void*)&m_net, sizeof(uint32_t));
}



// Data recvs
int message_recv_char(int socket, char* message)
{
    int ret = recv_all(socket, (void*)message, sizeof(char));
    return ret;
}

int message_recv_uint16_t(int socket, uint16_t* message)
{
    int ret = recv_all(socket, (void*)message, sizeof(uint16_t));
    *message = (uint16_t)ntohs(*message);
    return ret;
}

int message_recv_int16_t(int socket, int16_t* message)
{
    int ret = recv_all(socket, (void*)message, sizeof(int16_t));
    *message = (int16_t)ntohs(*message);
    return ret;
}

int message_recv_uint32_t(int socket, uint32_t* message)
{
    int ret = recv_all(socket, (void*)message, sizeof(uint32_t));
    *message = (uint32_t)ntohl(*message);
    return ret;
}

int message_recv_int32_t(int socket, int32_t* message)
{
    int ret = recv_all(socket, (void*)message, sizeof(int32_t));
    *message = (int32_t)ntohl(*message);
    return ret;
}