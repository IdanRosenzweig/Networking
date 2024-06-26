#include "interface_gateway.h"

interface_gateway::interface_gateway(const weak_ptr<iface_access_point> &access) : iface_access(access) {
    if (shared_ptr<iface_access_point> ptr = iface_access.lock())
        ptr->incoming_traffic.add_listener(this);
}

interface_gateway::~interface_gateway() {
    if (shared_ptr<iface_access_point> ptr = iface_access.lock())
        ptr->incoming_traffic.remove_listener(this);
}

int interface_gateway::send_data(send_msg_t &&data) {
    if (shared_ptr<iface_access_point> ptr = iface_access.lock())
        return ptr->send_out(data.get_active_buff(), data.get_count());
    else return 0;
}
