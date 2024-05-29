#ifndef NETWORKING_BLOCK_INCOMING_UDP_H
#define NETWORKING_BLOCK_INCOMING_UDP_H

#include <netinet/in.h>
#include <linux/if_ether.h>

#include "../../abstract/firewall/basic_firewall_filter.h"
#include "../../abstract/utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

class block_incoming_udp : public basic_firewall_filter<received_msg> {
    ip4_protocol ip_prot;
    udp_protocol udp_prot;

    class data_counter counter;

public:
    block_incoming_udp(uint16_t port) {
        ip_prot.listeners.append_new_empty_handler(&udp_prot);
        ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_UDP);

        udp_prot.listeners.append_new_empty_handler(&counter);
        udp_prot.listeners.add_requirement_to_last_handler<UDP_LISTEN_ON_PORT_INDEX>(port);
    }

    firewall_policy calc_policy(const received_msg& msg) override {
        received_msg copy = msg;
        ip_prot.handle_received_event(std::move(copy));

        int cnt = counter.get_count();
        counter.reset();

        if (cnt == 1) return BLOCK;
        else return ALLOW;
    }
};

#endif //NETWORKING_BLOCK_INCOMING_UDP_H
