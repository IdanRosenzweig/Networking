#include "ip_proxy_server.h"
#include "../../../protocols/ip4/internet_checksum.h"
#include <iostream>
using namespace std;

int conn_side_handler::send_data(send_msg msg) {
    return my_conn->send_data(msg);
}

void conn_side_handler::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    cout << "received proxied packet, size " << cnt << endl;
    if (cnt <= 0) return;

    struct iphdr * ip_hdr = reinterpret_cast<iphdr *>(buff);

    // store source ip
    ip4_addr source;
    extract_from_network_order(&source, (uint8_t*) &ip_hdr->saddr);
    my_source = source;

    // store dest ip
    ip4_addr dest_addr;
    extract_from_network_order(&dest_addr, (uint8_t*) &ip_hdr->daddr);

    // add to mappings
//    raw_tcp_server->mappings[dest_addr].insert(this);
    server->mappings.insert(dest_addr);

    // change source ip address
    ip4_addr new_source = server->server_ip;
    write_in_network_order((uint8_t*) &ip_hdr->saddr, &new_source);

    // update checksum
    ip_hdr->check = 0;
    ip_hdr->check = internet_checksum(reinterpret_cast<const uint16_t *>(buff), sizeof(iphdr));

    cout << "sending raw bytes to ip level" << endl;
    server->network_handler.send_data({buff, cnt});
}

int network_side_handler::send_data(send_msg msg) {
    return server->network_layer_gateway->send_data(msg);
}

void network_side_handler::handle_received_event(received_msg& msg) {
    // make sure there are connections to send the packet to
    // so no nullptr dereference or something like that happens
    if (server->conn_handler == nullptr) return;
    if (server->conn_handler->my_source == empty_ip4_addr) return;

    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;
    if (cnt <= 0) return;

    // get source ip
    ip4_addr source;
    extract_from_network_order(&source, (uint8_t*) &((struct iphdr *) buff)->saddr);

    // get dest ip
    ip4_addr dest_addr;
    extract_from_network_order(&dest_addr, (uint8_t*) &((struct iphdr *) buff)->daddr);

//    for (conn_side_handler* app_handler : raw_tcp_server->mappings[source]) {
//        struct iphdr* ip_hdr = reinterpret_cast<iphdr *>(buff);
//
//        // change the dest address
//        ip4_addr new_dest = app_handler->my_source;
//        write_in_network_order((uint8_t*) &ip_hdr->saddr, &new_dest);
//
//        // update checksum
//        ip_hdr->check = 0;
//        ip_hdr->check = internet_checksum(reinterpret_cast<const uint16_t *>(buff), sizeof(iphdr));
//
//        cout << "sending reply back to connection" << endl;
//        app_handler->send_data({buff, cnt});
//    }
    struct iphdr* ip_hdr = reinterpret_cast<iphdr *>(buff);

    // change the dest address
    ip4_addr new_dest = server->conn_handler->my_source;
    write_in_network_order((uint8_t*) &ip_hdr->saddr, &new_dest);

    // update checksum
    ip_hdr->check = 0;
    ip_hdr->check = internet_checksum(reinterpret_cast<const uint16_t *>(buff), sizeof(iphdr));

    cout << "sending reply back to connection " << convert_to_str(server->conn_handler->my_source) << "_" << endl;
    server->conn_handler->send_data({buff, cnt});

}