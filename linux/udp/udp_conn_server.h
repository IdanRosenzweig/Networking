#ifndef SERVERCLIENT_UDP_CONN_SERVER_H
#define SERVERCLIENT_UDP_CONN_SERVER_H

#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include "../../abstract/connectionless/basic_cl_server.h"
#include "../linux_cl_conn.h"
#include <memory>
#include <queue>
#include "../ip4/handler.h"

#include <arpa/inet.h>

class udp_conn_server : public basic_cl_server, public encapsulated_data_handler {
protected:
    // discover next host trying to send us data
    virtual sockaddr_in discover_next_host(); // todo in basic_cl_server with template

public:
//    std::queue<sockaddr_in> clients_q;

    sockaddr_in discover_next_client() {
//        sockaddr_in conn = discover_next_host();
//        clients_q.push(conn);
        return discover_next_host();
    }

    int port;
    int fd;

public:
    udp_conn_server(int port);

    void setup() override;

    void destroy() override;

    // last parameter gets IP. this should have been a PORT, but we don't use a sub class ip4_client...
    int send_encapsulated_data(void *buff, int count, sockaddr_in addr) ;
    // last parameter gets IP. this should have been a PORT, but we don't use a sub class ip4_client...
    int recv_encapsulated_data(void *buff, int count, sockaddr_in addr) ;

    void handler_received_data(void *buff) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H