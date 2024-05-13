#ifndef SERVERCLIENT_UDP_CLIENT_H
#define SERVERCLIENT_UDP_CLIENT_H

#include "../../abstract/connection/basic_connection.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"

class udp_client : public basic_connection {
public:
    basic_gateway* gateway;
    ip4_protocol ip_client;
    udp_protocol _udp_client;

    udp_client(ip4_addr dest_ip, int dest_port, int my_port, basic_gateway* gw = nullptr) {
        if (gw == nullptr) {
            gateway = new network_layer_gateway("enp0s3");
        } else
            gateway = gw;

        // setup send flow
        ip_client.gateway = gateway;
        ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
        ip_client.next_dest_addr.set_next_choice(dest_ip);
        ip_client.next_source_addr.set_next_choice(get_my_priv_ip_addr("enp0s3"));

        _udp_client.gateway = &ip_client;
        _udp_client.next_source_port.set_next_choice(my_port);
        _udp_client.next_dest_port.set_next_choice(dest_port);

        // setup recv flow
        gateway->add_listener(&ip_client);

        // todo change to both udp_client_server and tcpSession matching
        ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_client);

        _udp_client.port_handlers.assign_to_key(my_port, this);
    }

    int send_data(send_msg msg) override {
        return _udp_client.send_data(msg);
    }

};


#endif //SERVERCLIENT_UDP_CLIENT_H
