#ifndef SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H
#define SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H

#include "../../abstract/connection/basic_connection.h"

// takes a connection that doesn't necessarily preserve message boundary
// and wraps it with a message boundary preserving protocol.

// it also deletes all subprotocols that are stored in the messages coming from the connection (in msg.protocol_offsets)

template <typename MSG_SZ_FIELD = uint16_t>
class msg_boundary_seperator : public basic_connection {
    basic_connection* base_conn;
public:

    msg_boundary_seperator() {}
    explicit msg_boundary_seperator(basic_connection *baseConn) : base_conn(baseConn) {
        base_conn->add_listener(this);
    }

    int send_data(send_msg val) override {
        MSG_SZ_FIELD sz = val.count;

#define BUFF_SZ 1024
        char buff[BUFF_SZ];
        memset(buff, 0, BUFF_SZ);

        *(MSG_SZ_FIELD*) buff = sz;
        memcpy(buff + sizeof(MSG_SZ_FIELD), val.buff, sz);

        int total_sz = sz + sizeof(MSG_SZ_FIELD);
        return base_conn->send_data(send_msg{buff, total_sz});
    }

    bool mid_packet = false; // in the middle of reading a packet
    received_msg curr_msg; // curr constructing packet. contains the size of the curr packet and the allocated buffer for its data
    MSG_SZ_FIELD curr_read; // amount of bytes received so far for the curr packet

    void handle_received_event(received_msg &event) override {
        uint8_t *buff = event.data.get() + event.curr_offset;
        MSG_SZ_FIELD cnt = event.sz - event.curr_offset;

        while (cnt > 0) {
            if (!mid_packet) {
                MSG_SZ_FIELD curr_sz = *(MSG_SZ_FIELD*)buff; // todo fix assuming each send would contain at least sizeof(uint16_t) bytes
                buff += sizeof(MSG_SZ_FIELD);
                cnt -= sizeof(MSG_SZ_FIELD);

                curr_msg.data = std::unique_ptr<uint8_t>(new uint8_t[curr_sz]);
                curr_msg.sz = curr_sz;
                curr_msg.curr_offset = 0;
                curr_msg.protocol_offsets.clear();

                curr_read = 0;
                mid_packet = true;
            }

            MSG_SZ_FIELD read_left = curr_msg.sz - curr_read;
            MSG_SZ_FIELD reading = min(cnt, read_left);

            memcpy(curr_msg.data.get() + curr_read, buff, reading);
            curr_read += reading;
            buff += reading;
            cnt -= reading;

            if (curr_read == curr_msg.sz) {
                multi_receiver::handle_received_event(curr_msg);
                mid_packet = false;
            }
        }
    }
};


#endif //SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H