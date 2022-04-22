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
#include <unordered_map>
#include <vector>
#include <chrono>
#include <us/gov/bgtask.h>
#include <us/gov/socket/client.h>
#include "gc.h"

namespace us { namespace gov { namespace socket {

    struct client;
    struct daemon_t;

}}}

namespace us { namespace gov { namespace socket { namespace multipeer {

    struct clients_t final: unordered_map<sock_t, client*> { //neighbour ring
        using b = unordered_map<sock_t, client*>;
        using time_point = chrono::system_clock::time_point;

        clients_t(daemon_t&);
        clients_t(const clients_t&) = delete;
        ~clients_t();

        void add(client*);
        void update(vector<sock_t>& honest, vector<pair<host_t, string>>& evil);
        void dump(const string& prefix, ostream&) const;
        ko start();
        ko wait_ready(const time_point& deadline) const;
        void stop();
        void join();
        void feedlo();
        ko init(port_t self_port);
        void cleanup();

    private: //loopback
        ko connect0(const shostport_t&);
        ko connect0(const hostport_t&);
        ko init_sock2(const hostport_t&);

    public:
        mutable mutex mx;
        int locli;
        gc_t gc;
        daemon_t& daemon;
    };

}}}}

