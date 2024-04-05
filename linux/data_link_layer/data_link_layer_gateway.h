#ifndef SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
#define SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H

#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_gateway.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include <pcap/pcap.h>
#include <thread>
#include "../ip4/addit_data.h"
#include "../ether/mac_addr.h"
#include "data_link_traffic.h"

class data_link_layer_gateway : public basic_gateway {
private:
    int fd;
    struct ifreq if_idx;
    struct sockaddr_ll dest_addr; // used only for interface

    data_link_traffic traffic_in_sniff;

public:


    data_link_layer_gateway();

    ~data_link_layer_gateway() override;

    int send_raw(void *buff, int count) override;

    int recv_raw(void *buff, int count) override;


};
#endif //SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
