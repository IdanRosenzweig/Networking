#ifndef SERVERCLIENT_IP4_CONN_SERVER_H
#define SERVERCLIENT_IP4_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/basic_encapsulating_server.h"
#include <memory>
#include "handler.h"

#include <arpa/inet.h>
#include <map>
#include <queue>
#include <string>

#include "../../abstract/protocol_multiplexer.h"
#include "../ether/ethernet_conn_server.h"

struct ip4_addr {
    uint32_t raw;
};

class ip4_conn_server : public basic_encapsulating_server<int, ip4_addr> {
public:
    ip4_conn_server();

    protocol_multiplexer<int, circular_buffer<message, MAX_NO_MSG>> protocolQueue;

    ethernet_conn_server *ether_server;

    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
