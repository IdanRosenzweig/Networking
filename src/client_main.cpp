#include <iostream>
#include <vector>
#include <linux/if_ether.h>
#include <pcap/pcap.h>
#include <thread>
#include <net/if.h>
#include <sys/ioctl.h>

#include "protocols/udp/udp_protocol.h"
#include "temp_connections/dns_server/dns_client.h"
#include "protocols/ip4/ip4_protocol.h"
#include "protocols/icmp/icmp_protocol.h"
#include "protocols/ether/ethernet_protocol.h"
#include "protocols/arp/arp_protocol.h"

#include "linux/data_link_layer/data_link_layer_gateway.h"
#include "protocols/udp/udp_protocol.h"
#include "proxy/network_layer/ip_proxy_client.h"
#include "abstract/circular_buffer.h"
#include "abstract/receiving/recv_queue.h"
#include "temp_connections/udp/udp_client.h"
#include "protocols/tcp/tcp_protocol.h"

#include "abstract/firewall/firewall.h"
#include "firewalls/network_layer/only_udp_filter.h"
#include "analyzer/analyzer.h"

#include "linux/virtual_if.h"

#include "temp_connections/ssh/ssh_conn_session.h"
#include "vpn/common.h"
#include "vpn/vpn_client.h"

#define MY_IP "10.100.102.18"

class udp_app_client : public recv_queue<received_msg> {
public:
    msg_sender *gateway;

    void print() {
        received_msg msg = front_data();
        cout << "daemon: " << (char *) (msg.data.get() + msg.curr_offset) << endl;
    }

    void send(string str) {
        gateway->send_data({(void *) str.c_str(), (int) str.size()});
    }
};


void udp_main() {

    constexpr int my_port = 1212;
    constexpr int server_port = 4444;
    string server_addr{"10.100.102.31"};

    // network layer gateway protected by firewall
    network_layer_gateway networkLayerGateway;

//    firewall fw(&networkLayerGateway);
//    fw.filters.push_back(new only_udp_filter<my_port>());

    // protocol stack
    ip4_protocol ip_client;
    udp_protocol udp_client;

    udp_app_client app_system;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(server_addr));
    ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    udp_client.gateway = &ip_client;
    udp_client.next_source_port.set_next_choice(my_port);
    udp_client.next_dest_port.set_next_choice(server_port);

    app_system.gateway = &udp_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

    udp_client.port_handlers.assign_to_key(my_port, &app_system);

    // communicate
    for (int i = 0; i < 5; i++) {
        app_system.send("clint");
        app_system.print();
    }

    while (true) {

    }
}

void tcp_main() {
    std::cout << "Hello, World!" << std::endl;

    // protocol stack
    tcp_protocol tcp_client;

    // setup send flow
    tcp_client.next_addr.set_next_choice(convert_to_ip4_addr("127.0.0.1"));
    tcp_client.next_dest_port.set_next_choice(5678);
    tcp_client.next_source_port.set_next_choice(6666);


    // comm
    tcp_client.start_session();

    while (tcp_client.as_client_sessions.empty()) {

    }

    // has session
    cout << "has session" << endl;
    std::unique_ptr<basic_session> &session = tcp_client.as_client_sessions[0];

    sleep(3);

    std::cout << "sending msg" << std::endl;
    char *msg = "hello";
    cout << "sent " << session->send_data({msg, (int) strlen(msg)}) << " bytes" << endl;
    sleep(1);

    msg = "thisis test msg";
    cout << "sent " << session->send_data({msg, (int) strlen(msg)}) << " bytes" << endl;
    sleep(1);

    msg = "another one";
    cout << "sent " << session->send_data({msg, (int) strlen(msg)}) << " bytes" << endl;

    while (true) {

    }

}

void dns_main() {
//    string server_addr = "10.100.102.18";
    string server_addr = "8.8.8.8";

    vector<string> hosts = {
            "wiki.osdev.org",
            "www.wix.com",
            "docs.google.com",
            "www.scs.stanford.edu",
            "yahoo.com",
            "google.com",
            "youtube.com",
            "tradingview.com",
            "walla.co.il",
            "nasa.com",
            "medium.com",
            "www.scs.stanford.edu",
            "docs.google.com",
            "slides.google.com",
            "sheets.google.com",
            "podcasts.google.com",
            "gmail.google.com",
            "account.google.com",
    };

    // protocol stack
    network_layer_gateway networkLayerGateway;

    ip4_protocol ip_client;
    udp_protocol udp_client;
    dns_client dns_client;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(server_addr));
    ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    udp_client.gateway = &ip_client;
    udp_client.next_source_port.set_next_choice(1212);
    udp_client.next_dest_port.set_next_choice(DNS_SERVER_PORT);

    dns_client.gateway = &udp_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

    udp_client.port_handlers.assign_to_key(1212, &dns_client);

    // communicate
    for (string &str: hosts) {
        dns_client.query({str});
        cout << endl << endl << endl;
    }

//    while (true) {}

}

void icmp_main() {
    char *str = "172.217.22.46";
//    char* str = "google.com";

    // protocol stack
    network_layer_gateway networkLayerGateway;

    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
    ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    icmp_client.gateway = &ip_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

    // ping
    icmp_client.ping();
}

void arp_main() {

    mac_addr my_mac = get_my_mac_address("enp0s3");
    ip4_addr my_ip = get_my_priv_ip_addr("enp0s3");

    // protocol stack
    data_link_layer_gateway dataLinkLayerGateway;

    ethernet_protocol ether_client;
    arp_protocol arp_client;

    // for sending
    ether_client.gateway = &dataLinkLayerGateway;
    ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
    ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);
    ether_client.next_source_addr.set_next_choice(my_mac);

    arp_client.gateway = &ether_client;

    // for receiving
    dataLinkLayerGateway.add_listener(&ether_client);
    ether_client.protocol_handlers.assign_to_key(htons(ETH_P_ARP), &arp_client);


    // attack
//    mac_addr res = arp_client.search_for_mac_addr("10.100.102.27");
//    print_mac(res);



    string victim = "10.100.102.15";
    vector<pair<mac_addr, string>> victim_list = {
//            {arp_client.search_for_mac_addr(victim, my_mac, my_ip), victim}
    };
    arp_client.spoof_as_device("10.100.102.1", // router
                               my_mac,
                               victim_list);

}

void proxy_main() {

    string proxy_server = "10.100.102.18";

    // first node
    udp_client udp_1(proxy_server, 4001, 1001);
//    icmp_connection icmpConnection;
    ip_proxy_client proxy_1(&udp_1);

    // second node
//    udp_prot udp_2("10.100.102.18", 4002, 1002, &proxy_1);
////    icmp_connection icmpConnection;
//    ip_proxy_client proxy_2(&udp_2);

    // third node
//    _udp_client udp_3("10.100.102.18", 4003, 1003, &proxy_2);
////    icmp_connection icmpConnection;
//    ip_proxy_client proxy_3(&udp_3);

//     regular dns communication
//    {
//
//        ip4_protocol ip_prot;
//        udp_protocol udp_prot;
//        dns_client dns_client;
//
//        // setup send flow
//        ip_prot.gateway = &proxy_1;
//        ip_prot.next_protocol.set_next_choice(IPPROTO_UDP);
//        ip_prot.next_dest_addr.set_next_choice(convert_to_ip4_addr("8.8.8.8"));
//        ip_prot.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));
//
//        udp_prot.gateway = &ip_prot;
//        udp_prot.next_source_port.set_next_choice(4545);
//        udp_prot.next_dest_port.set_next_choice(DNS_PORT);
//
//        dns_client.gateway = &udp_prot;
//
//        // setup recv flow
//        proxy_1.add_listener(&ip_prot);
//
//        ip_prot.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_prot);
//
//        udp_prot.port_handlers.assign_to_key(4545, &dns_client);
//
//        // communicate
//        // send msg
//        vector<string> hosts = {
//                "wiki.osdev.org",
//                "www.wix.com",
//                "docs.google.com",
//                "www.scs.stanford.edu",
//                "yahho.com",
//                "google.com",
//                "youtube.com",
//                "tradingview.com",
//                "walla.co.il",
//                "nasa.com",
//                "medium.com",
//                "www.scs.stanford.edu",
//        };
//
//        for (string& str : hosts)
//            dns_client.query(str);
//
//    }


    // icmp
    {
        char *str = "172.217.22.46";
        //    char* str = "google.com";

        ip4_protocol ip_client;
        icmp_protocol icmp_client;

        // setup send flow
        ip_client.gateway = &proxy_1;
        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

        icmp_client.gateway = &ip_client;

        // setup recv flow
        proxy_1.add_listener(&ip_client);

        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

        // ping
        icmp_client.ping();
    }

}

void sniffer_main() {
    analyzer packetAnalyzer;

    while (true) {

    }
}


void vpn_main() {
    string vpn_server = "10.100.102.31";
    vpn_client vpn(vpn_server);

    char dev[6] = "virt0";
    linux_virtual_iface iface(&vpn, dev);
    while (true) {

    }

    // icmp
//    {
//        char *str = "172.217.22.46";
//        //    char* str = "google.com";
//
//        ethernet_protocol ether_client;
//        ip4_protocol ip_client;
//        icmp_protocol icmp_client;
//
//        // setup send flow
//        ether_client.gateway = &client;
//        ether_client.next_protocol.set_next_choice(htons(ETH_P_IP));
//        ether_client.next_dest_addr.set_next_choice(
//                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
//        );
//        ether_client.next_source_addr.set_next_choice(get_my_mac_address("enp0s3"));
//
//        ip_client.gateway = &ether_client;
//        ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
//        ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(str));
////        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));
//        ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.18"));
//
//        icmp_client.gateway = &ip_client;
//
//        // setup recv flow
//        client.add_listener(&ether_client);
//
//        ether_client.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip_client);
//
//        ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);
//
//        // ping
//        icmp_client.ping();
//    }

//        regular dns communication
//    {
//        ethernet_protocol ether_prot;
//        ip4_protocol ip_prot;
//        udp_protocol udp_prot;
//        dns_client dns_client;
//
//        // setup send flow
//        ether_prot.gateway = &client;
//        ether_prot.next_protocol.set_next_choice(htons(ETH_P_IP));
//        ether_prot.next_dest_addr.set_next_choice(
//                {0xc4, 0xeb, 0x42, 0xed, 0xc5, 0xb7} // gateway
//        );
//        ether_prot.next_source_addr.set_next_choice(get_my_mac_address("enp0s3"));
//
//        ip_prot.gateway = &ether_prot;
//        ip_prot.next_protocol.set_next_choice(IPPROTO_UDP);
//        ip_prot.next_dest_addr.set_next_choice(convert_to_ip4_addr("8.8.8.8"));
//        ip_prot.next_source_addr.set_next_choice(convert_to_ip4_addr("10.100.102.31"));
//
//        udp_prot.gateway = &ip_prot;
//        udp_prot.next_source_port.set_next_choice(4545);
//        udp_prot.next_dest_port.set_next_choice(DNS_SERVER_PORT);
//
//        dns_client.gateway = &udp_prot;
//
//        // setup recv flow
//        client.add_listener(&ether_prot);
//
//        ether_prot.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip_prot);
//
//        ip_prot.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_prot);
//
//        udp_prot.port_handlers.assign_to_key(4545, &dns_client);
//
//        // communicate
//        // send msg
//        vector<string> hosts = {
//                "wiki.osdev.org",
//                "www.wix.com",
//                "docs.google.com",
//                "www.scs.stanford.edu",
//                "yahho.com",
//                "google.com",
//                "youtube.com",
//                "tradingview.com",
//                "walla.co.il",
//                "nasa.com",
//                "medium.com",
//                "www.scs.stanford.edu",
//        };
//
//        for (string& str : hosts)
//            dns_client.query(str);
//
//    }

}

//void ssh_main() {
//    ssh_conn_session client("10.100.102.18", "idan", "123", 1234);
//
//    cout << "ssh client started" << endl;
//
////    client.execute_remote_cli_command("ls -la");
////    client.execute_remote_cli_command("id");
//    send_msg msg;
//    char buff[10] = "hello";
//    msg.buff = buff;
//    msg.count = 10;
//    cout << "sent " << client.send_data(msg) << endl;
//
//    while (true) {
//
//    }
//}

int main() {

//    udp_main();
//    tcp_main();
//    dns_main();
//    icmp_main();
//    arp_main();
//    proxy_main();
//    sniffer_main();
    vpn_main();

    return (0);
}
