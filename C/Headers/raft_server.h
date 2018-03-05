//
// Created by ubuntu on 24/02/18.
//

#ifndef AMIRPROJEXT_MULTICAST_MESSAGE_H
#define AMIRPROJEXT_MULTICAST_MESSAGE_H


#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#include <stdint.h>
#include <netinet/in.h>
#include <string.h>

#include "general.h"


typedef struct sender{
    int fd;
    struct sockaddr_in addr;
}Sender;

typedef struct listener{
    struct ip_mreq mreq;
    struct sockaddr_in addr;
    int fd;
}Listener;

typedef struct multicat_message{
    Sender multicast_sender;
    Listener multicast_listener;

}MulticastMessage;

extern MulticastMessage multicastMessage;

int init_multicast_message(const char *ip, uint16_t port);
int send_raft_message(void *message_obj, int message_size);
int get_raft_message(void *message_memory_obj);
//ErrorMessage delete_multicast_message(MulticastMessage *multicastMessage);

#endif //AMIRPROJEXT_MULTICAST_MESSAGE_H
