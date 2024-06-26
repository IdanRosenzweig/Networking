#ifndef NETWORKING_SEND_MSG_T_H
#define NETWORKING_SEND_MSG_T_H

#include "../basic_send_medium.h"
#include "../send_forwarder.h"

#include <stdint.h>
#include <cstring>

// a buffer for sending packets.

// active_buff represents the current buffer of the packet. count represents the current size of buff in active_buff.

// reserve_buff is used when needed to encapsulate the existing packet in active_buff.
// to do so, read the buff in active_buff, encapsulate it in the new protocol you want,
// and write the result into reserve_buff.
// then just call toggle_active_buff() and set_count() accordingly. this would swap the active_buff and reserve_buff.

// ##### THE INITIAL STATE OF SEND_MSG WILL PROBABLY NOT BE PRESERVED AFTER DOING THIS PROCEDURE MULTIPLE TIMES #####

#define SEND_MSG_MTU 65000

class send_msg_t {
    uint8_t buff1[SEND_MSG_MTU] = {0};
    uint8_t buff2[SEND_MSG_MTU] = {0};
    int count = 0;
    bool toggle = false; // 0: active is buff1, 1: active is buff2

public:
    inline uint8_t *get_active_buff() {
        return toggle == 0 ? buff1 : buff2;
    }

    inline uint8_t *get_reserve_buff() {
        return toggle == 0 ? buff2 : buff1;
    }

    inline void toggle_active_buff() { toggle = !toggle; }

    inline int get_count() { return count; }

    inline void set_count(int new_cnt) { count = new_cnt; }

    void reset() {
        memset(buff1, 0, sizeof(buff1));
        memset(buff2, 0, sizeof(buff2));
        count = 0;
        toggle = false;
    }
};

using msg_send_medium = basic_send_medium<send_msg_t>;
using msg_send_forwarder = send_forwarder<send_msg_t>;

#endif //NETWORKING_SEND_MSG_T_H
