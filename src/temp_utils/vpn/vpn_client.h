#ifndef NETWORKING_VPN_CLIENT_H
#define NETWORKING_VPN_CLIENT_H

#include "../../abstract/gateway/gateway.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_client.h"
#include "../../protocols/msg_boundary/msg_boundary_seperator.h"

#include "common.h"

class vpn_client : public gateway {
    tcp_client tcp;
    msg_boundary_seperator<> client;

public:
    vpn_client(ip4_addr daemon_ip) :
            tcp(daemon_ip, 5678, 1212), client(&tcp) {
        client.add_listener(this);
//tcp_client_server.add_listener(this);
    }

    int send_data(send_msg<>&& val) override {
        return client.send_data(std::move(val));
//        return tcp_client_server.send_data(val);
    }
};


#endif //NETWORKING_VPN_CLIENT_H
