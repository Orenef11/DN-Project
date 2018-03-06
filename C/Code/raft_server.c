//
// Created by ubuntu on 24/02/18.
//

#include "../Headers/raft_server.h"

MulticastMessage multicastMessage = {0};

int init_multicast_listener(const char *ip, uint16_t port){

    memset(&multicastMessage.multicast_listener,0,sizeof(multicastMessage.multicast_listener));

    int option_value;
    /* create what looks like an ordinary UDP socket */
    if ((multicastMessage.multicast_listener.fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        return errno;
    }


/**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(multicastMessage.multicast_listener.fd, SOL_SOCKET, SO_REUSEADDR, &option_value,
                   sizeof(option_value)) < 0) {
        perror("Reusing ADDR failed");
        return errno;
    }
/*** END OF MODIFICATION TO ORIGINAL */

    /* set up destination address */
    memset(&multicastMessage.multicast_listener.addr, 0, sizeof(multicastMessage.multicast_listener.addr));
    multicastMessage.multicast_listener.addr.sin_family = AF_INET;
    multicastMessage.multicast_listener.addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
    multicastMessage.multicast_listener.addr.sin_port = htons(port);

    /* bind to receive address */
    if (bind(multicastMessage.multicast_listener.fd, (struct sockaddr *) &multicastMessage.multicast_listener.addr,
             sizeof(multicastMessage.multicast_listener.addr)) < 0) {
        perror("bind");
        return errno;
    }

    /* use setsockopt() to request that the kernel join a multicast group */
    multicastMessage.multicast_listener.mreq.imr_multiaddr.s_addr = inet_addr(ip);
    multicastMessage.multicast_listener.mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(multicastMessage.multicast_listener.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &multicastMessage.multicast_listener.mreq, sizeof(multicastMessage.multicast_listener.mreq)) < 0) {
        perror("setsockopt");
        return errno;
    }
}

int init_multicast_sender(const char *ip, uint16_t port){

    memset(&multicastMessage.multicast_sender,0,sizeof(multicastMessage.multicast_sender));

    if ((multicastMessage.multicast_sender.fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) < 0) {
        perror("socket");
        return errno;
    }

    /* set up destination address */
    memset(&multicastMessage.multicast_sender.addr,0,sizeof(multicastMessage.multicast_sender.addr));
    multicastMessage.multicast_sender.addr.sin_family=AF_INET;
    multicastMessage.multicast_sender.addr.sin_addr.s_addr=inet_addr(ip);
    multicastMessage.multicast_sender.addr.sin_port=htons(port);
}


int init_multicast_message(const char *ip, uint16_t port){
#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"init multicast socket",NO_VALUES,0);
#endif

    init_multicast_listener(ip, port);
    init_multicast_sender(ip, port);

#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"multicast was opened successfully",NO_VALUES,0);
#endif
    return 0;
}

int send_raft_message(void *message_obj, int message_size){

#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"try to send msg",INT_VALUES,1,LOG(message_size));
#endif


    if (sendto(multicastMessage.multicast_sender.fd, &message_size, sizeof(message_size), 0, (struct sockaddr *)
            &multicastMessage.multicast_sender.addr, sizeof(multicastMessage.multicast_sender.addr)) < 0) {
        return errno;
    }

    if (sendto(multicastMessage.multicast_sender.fd, message_obj, message_size, 0, (struct sockaddr *)
            &multicastMessage.multicast_sender.addr, sizeof(multicastMessage.multicast_sender.addr)) < 0) {
        return errno;
    }


#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"msg was successfully sent",INT_VALUES,1,LOG(message_size));
#endif
    return 0;
}

int get_raft_message(void *message_memory_obj){

#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"try to read message",NO_VALUES,0);
#endif

    int message_size;
    int addrlen = sizeof(multicastMessage.multicast_listener.addr);

    if ((recvfrom(multicastMessage.multicast_listener.fd , &message_size, sizeof(message_size), 0,
                  (struct sockaddr *) &multicastMessage.multicast_listener.addr,
                  &addrlen)) < 0) {
        return errno;
    }


    if ((recvfrom(multicastMessage.multicast_listener.fd , message_memory_obj, message_size , 0,
                  (struct sockaddr *) &multicastMessage.multicast_listener.addr,
                  &addrlen)) < 0) {
        return errno;
    }

#if DEBUG_MODE == 1
    WRITE_TO_LOGGER(DEBUG_LEVEL,"msg was successfully read",INT_VALUES,1,LOG(message_size));
#endif
    return 0;
}
