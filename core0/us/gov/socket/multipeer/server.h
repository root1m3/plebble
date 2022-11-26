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
#include <us/gov/bgtask.h>
#include <us/gov/socket/types.h>
#include <us/gov/socket/busyled_t.h>

namespace us::gov::socket {
    struct datagram;
    struct client;
    struct daemon_t;
}

namespace us::gov::socket::multipeer {

    struct server0 {
        virtual void receive_and_process(client*) = 0;
    };

    struct server: bgtask, server0 {
        using b = bgtask;

        struct blacklist: unordered_map<host_t, pair<time_point, string>> {
            static constexpr chrono::seconds bantime{300}; //5m;
            void add(host_t, const reason_t&);
            void purge();
            void dump(ostream&) const;
        };

        server(daemon_t&, port_t port);
        virtual ~server();

    public:
        void proc_evil(const vector<pair<host_t, string>>&);
        void onwakeup();
        void run_recv();
        void dump(const string& prefix, ostream&) const;

    private:
        datagram* read_from_client(sock_t);
        sock_t make_socket(port_t);
        bool banned_throttle(const string& addr);

    public:
        port_t port;
        sock_t sock{0};
        blacklist ban;
        busyled_t busyled;
        daemon_t& daemon;

        #if CFG_COUNTERS == 1
            struct counters_t {
                uint32_t select_cycles{0};
                uint32_t select_errors{0};
                uint32_t select_EINTR{0};
                uint32_t select_EBADF{0};
                uint32_t new_connections{0};
                uint32_t closed_banned{0};
                uint32_t data_arrived_unmapped_fd{0};
                uint32_t blacklisted_addr{0};
                uint32_t disconnection_normal{0};
                uint32_t disconnection_unknown_service{0};
                void dump(ostream&) const;
            };
            static counters_t counters;
        #endif

        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

