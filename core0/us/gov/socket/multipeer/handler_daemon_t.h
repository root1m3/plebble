//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
#include "server.h"
#include "thpool.h"

namespace us { namespace gov { namespace socket {

    struct peer_t;
    struct client;

}}}

namespace us { namespace gov { namespace socket { namespace multipeer {

    struct clients_t;

    struct handler_daemon_t: server {
        using b = server;
        using pool_t = thpool;
        using peers_t = clients_t;

        static const char* KO_42093;

        handler_daemon_t(daemon_t&, port_t port, uint8_t workers);
        handler_daemon_t(const handler_daemon_t&) = delete;
        handler_daemon_t& operator = (const handler_daemon_t&) = delete;
        ~handler_daemon_t() override;

    public:
        virtual bool process_unhandled(client&, datagram*);

    public:
        ko start();
        void stop();
        void join();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;

    public:
        void receive_and_process(client*) final override;
        void set_busy_handler(busyled_t::handler_t*);


    public:
        void dump(const string& prefix, ostream&) const;

    public:
        pool_t pool;
        uint8_t num_workers;
    };

}}}}

