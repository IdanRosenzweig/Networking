#ifndef NETWORKING_DNS_CLIENT_H
#define NETWORKING_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/sending/msg/send_msg.h"
#include "../../abstract/gateway/gateway.h"

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/emp/emp.h"

#include "../../temp_prot_stacks/udp_client_server/udp_client.h"
#include "../../temp_prot_stacks/emp/emp_client.h"
#include "dns.h"

class dns_client : public recv_queue<received_msg> {
public:

    udp_client udpClient;
//    emp_client empClient;

    dns_client(ip4_addr dest_server_ip, ip4_addr src_ip, gateway* network_layer_gw);

    void query(dns_record_type type, const std::string& key);

};


#endif //NETWORKING_DNS_CLIENT_H