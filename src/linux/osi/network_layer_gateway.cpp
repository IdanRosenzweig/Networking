#include "network_layer_gateway.h"

#include "../if/hardware.h"

#include <iostream>
using namespace std;

network_layer_gateway::network_layer_gateway(const weak_ptr<iface_access_point>& access) : dataLinkLayerGateway(access), arp_handler(&dataLinkLayerGateway) {

    mac_addr my_mac;
    ip4_addr my_ip;
    ip4_addr default_gw;
    if (shared_ptr<iface_access_point> ptr = dataLinkLayerGateway.iface_access.lock()) {
        my_mac = get_mac_addr_of_iface(ptr->iface_name);
        my_ip = get_ip_addr_of_iface(ptr->iface_name);
        default_gw = get_default_gateway_of_iface(ptr->iface_name);
    } else return;
//    std::cout << "default gateway is " << convert_to_str(default_gw) << endl;

    // send
    ether_prot.send_medium.add_send_channel(&dataLinkLayerGateway);
    ether_prot.next_protocol.set_next_choice(ethernet_header::ethertype_values::ip4);
    ether_prot.next_dest_addr.set_next_choice(
            arp_handler.search_for_mac_addr(default_gw, my_mac, my_ip)
    );
    ether_prot.next_source_addr.set_next_choice(my_mac);

    // recv
    dataLinkLayerGateway.add_listener(&ether_prot);

    ether_prot.listeners.append_new_empty_handler(this);
    ether_prot.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_PROTOCOL_INDEX>(ethernet_header::ethertype_values::ip4);
//    ether_prot.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_DEST_ADDR_INDEX>(my_mac);
}

network_layer_gateway::~network_layer_gateway() {
    dataLinkLayerGateway.remove_listener(&ether_prot);
}

int network_layer_gateway::send_data(send_msg_t &&data) {
    return ether_prot.send_data(std::move(data));
}

void network_layer_gateway::handle_callback(recv_msg_t &&data) {
    recv_forwarder::handle_callback(std::move(data));
}


