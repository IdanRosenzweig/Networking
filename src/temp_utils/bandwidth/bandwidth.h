#ifndef SERVERCLIENT_BANDWIDTH_H
#define SERVERCLIENT_BANDWIDTH_H

#include "../../abstract/connection/basic_connection.h"
#include "../../abstract/connection/multi_sniffer.h"
#include "../../abstract/gateway/basic_gateway.h"

class bandwidth {
    multi_sniffer* conn;

    class my_sniff : public basic_sniffer {
        bandwidth* master;
    public:
        explicit my_sniff(bandwidth *master) : master(master) {}

        void handle_outgoing_packet(received_msg &msg) override;

        void handle_incoming_packet(received_msg &msg) override;
    };
    my_sniff sniff;


public:
    explicit bandwidth(multi_sniffer *conn);

    uint64_t bytes_out_cnt = 0;
    uint64_t bytes_in_cnt = 0;


};


#endif //SERVERCLIENT_BANDWIDTH_H