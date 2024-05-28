#ifndef SERVERCLIENT_BLOCK_INCOMING_TCP_H
#define SERVERCLIENT_BLOCK_INCOMING_TCP_H

#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>

#include "../../abstract/firewall/basic_firewall_filter.h"
#include "../../abstract/utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

class block_incoming_tcp : public basic_firewall_filter<received_msg> {
    ethernet_protocol ether_prot;
    ip4_protocol ip_prot;

    class examiner : public msg_receiver {
        block_incoming_tcp *master;
    public:
        explicit examiner(block_incoming_tcp *master) : master(master) {}

    public:
        void handle_received_event(received_msg &&event) override {
            struct tcphdr *tcp_hdr = reinterpret_cast<tcphdr *>(event.data.data() + event.curr_offset);
            int dest_port = ntohs(tcp_hdr->dest);
//            std::cout << "tcp examiner got " << dest_port << endl;
            if (dest_port == master->blocked_port) {
//                std::cout << "blocking" << endl;
                master->blocked_flag = true;
            }
        }

    } tcp_examine;

    int blocked_port;
    bool blocked_flag = false;

public:
    block_incoming_tcp(int port) : blocked_port(port), tcp_examine(this) {
        ether_prot.protocol_handlers[htons(ETH_P_IP)].push_back( &ip_prot);
        ip_prot.protocol_handlers[IPPROTO_TCP].push_back( &tcp_examine);
    }

    firewall_policy calc_policy(const received_msg &msg) override {
        received_msg copy = msg;
        ether_prot.handle_received_event(std::move(copy));

        if (blocked_flag) {
            blocked_flag = false;
//            std::cout << "policy set blocked" << endl;
            return BLOCK;
        }
        else return ALLOW;
    }
};

#endif //SERVERCLIENT_BLOCK_INCOMING_TCP_H