#ifndef SERVERCLIENT_SESSION_MANAGER_H
#define SERVERCLIENT_SESSION_MANAGER_H

#include "session_conn.h"
#include "session_generator.h"
#include "../receiving/msg/msg_receiver.h"
#include "session_handler.h"
#include <memory>
#include <vector>

// class that takes a session generator and manages all its generated sessions.
// creates a session handler for each new session and instantiates it
template<typename SESSION_CONN, typename SESSION_HANDLER>
class session_manager {
    static_assert(std::is_base_of_v<session_handler<SESSION_CONN>, SESSION_HANDLER>,
                  "type of SESSION_HANDLER must be derived type of session_handler<SESSION_CONN>");

    struct internal_session_handler {
        session_t<SESSION_CONN> session;
        SESSION_HANDLER handler;

    public:
        explicit internal_session_handler(session_t<SESSION_CONN> &&sess)
                : session(std::move(sess)), handler(session) {
            handler.on_session_start();
        }
    };

    struct sessions_recv : basic_receiver<session_t<SESSION_CONN>> {
        session_manager *master;

        void handle_received_event(session_t<SESSION_CONN> &&event) override {
            master->sessions.emplace_back(std::move(event));
        }

        explicit sessions_recv(session_manager *master) : master(master) {}
    };

    sessions_recv receiver;

public:
    std::vector<internal_session_handler> sessions;

    session_manager() = delete;

    session_manager(session_generator<SESSION_CONN> *sessions_generator) : receiver(this) {
        sessions_generator->set_generator_listener(&receiver);
    }

};

#endif //SERVERCLIENT_SESSION_MANAGER_H