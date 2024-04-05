#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <queue>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include "../ip4/addit_data.h"
#include "mac_addr.h"
#include "../data_link_layer/data_link_layer_gateway.h"
#include "../../abstract/protocol_queue.h"

mac_addr get_my_mac_address(const char *interface_name);

void print_mac(mac_addr addr);

std::string get_my_priv_ip(const char *interface_name);

class ethernet_conn_client : public basic_cl_client, public basic_encapsulating_client<int, prot_addit_data> {
private:
// instead of listening and filtering encapsulated protocols ourselves,
// linux will do it for us. just create a file descriptor for the dedicated protocol
//    void register_filter(int prot);


//    struct ifreq if_idx;
//    struct sockaddr_ll dest_addr; // used for interface

    data_link_layer_gateway gateway;

    std::thread worker;

    protocol_queue<int> protocolQueue;

public:

    mac_addr my_mac;
    struct ifreq my_priv_ip;

    mac_addr dest_device = BROADCAST_MAC;
    void change_dest_mac(mac_addr mac);

    ethernet_conn_client();

    virtual ~ethernet_conn_client();

    void init() override;

    void finish() override;


    // receive the next msg of the encapsulated protocol
    int recv_next_msg( void* buff, int count) override;

    // send message to the last client that sent message with the protocol
    int send_next_msg( void* buff, int count) override;

//    void spoof();

};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
