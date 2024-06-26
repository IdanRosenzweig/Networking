#ifndef NETWORKING_TCP_SESSION_H
#define NETWORKING_TCP_SESSION_H

#include <unistd.h>
#include <thread>

#include "../../abstract/session/session_conn.h"
#include "../ip4/ip4_addr.h"
#include "../ip4/ip4_protocol.h"

// things related to the logical tcp session
struct tcp_session_data {
    ip4_addr dest_addr;
    uint16_t source_port;
    uint16_t dest_port;
};

// things related to the pure network connection
class tcp_session_conn : public session_conn {

    int sd;
    std::thread worker;

public:

    bool alive;

    tcp_session_conn() {}
    tcp_session_conn(int sd);

    ~tcp_session_conn();

    int send_data(send_msg_t&&data) override;

    void handle_callback(recv_msg_t&& data) override;

};


#endif //NETWORKING_TCP_SESSION_H
