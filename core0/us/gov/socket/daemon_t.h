//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
#pragma once
#include <utility>
#include <us/gov/ko.h>
#include <us/gov/bgtask.h>
#include "peer_t.h"
#include "datagram.h"
#include "types.h"
#include "multipeer/clients.h"
#include "multipeer/handler_daemon_t.h"
#include "daemon0_t.h"

namespace us::gov::socket {

    struct daemon_t: daemon0_t, multipeer::handler_daemon_t {
        using b = daemon0_t;
        using caller = b;
        using handler = multipeer::handler_daemon_t;

        using dispatcher_t = datagram::dispatcher_t;
        using peers_t = multipeer::clients_t;

        daemon_t(channel_t, port_t, uint8_t workers); /// mode_multipeer constructor
        ~daemon_t() override;

        bool process_unhandled(client&, datagram*) final override;
        inline busyled_t& recv_led() final override { return handler::busyled; }
        virtual void on_destroy_(client&) {}
        virtual client* create_client(sock_t sock);
        inline void wakeup_handler() { handler::task_wakeup(); }
        inline void wakeup_caller() { caller::task_wakeup(); }
        inline void wakeup_gc() { peers.gc.task_wakeup(); }
        void new_peer(int fd);
        void attach(client*) final override;
        void detach(client&) final override;

    public:
        inline bool isup() const { return handler::isup() && caller::isup(); }
        inline bool isdown() const { return handler::isdown() || caller::isdown(); }
        inline bool resumed() const { return handler::resumed() || caller::resumed(); }
        inline void reset() { handler::reset(); caller::reset(); }
        inline bool is_active() const { return handler::is_active() && caller::is_active(); }
        void set_busy_handlers(busyled_t::handler_t* hsend, busyled_t::handler_t* hrecv);

        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;
        void stop();
        void join();

        static int set_not_blocking_mode(int sock);
        inline pair<ko, datagram*> sendrecv(client& peer, datagram* d, string& remote_error) { return caller::rendezvous.sendrecv(peer, d, remote_error); }

    public:
        void dump(const string& prefix, ostream&) const;
        virtual void dump_all(const string& prefix, ostream&) const;

    public:
        peers_t peers;

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

