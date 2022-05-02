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
#include "net.h"
#include <vector>
#include <unordered_set>
#include <string>

namespace us::gov::engine {

    struct daemon_t;
    struct local_deltas_t;
    struct peer_t;
    struct evidence;

    struct net_daemon_t: net::daemon_t {
        using b = net::daemon_t;

        net_daemon_t(engine::daemon_t&, channel_t);
        net_daemon_t(engine::daemon_t&, channel_t, port_t, pport_t, uint8_t edges, uint8_t devices, int workers, const vector<hostport_t>& seed_nodes);
        ~net_daemon_t() override;

        net_daemon_t(const net_daemon_t&) = delete;
        net_daemon_t& operator = (const net_daemon_t&) = delete;

        socket::peer_t* create_client(int sock) override;
        bool check_dfs_permission(const hash_t& addr) const override;
        bool check_relay_permission(const hash_t& addr) const override;
        const keys& get_keys() const override;
        virtual string homedir() const;

        const function<bool(const socket::client&)>& sysop_filter() const;
        static const function<bool(const socket::client&)>& node_filter();
        static const function<bool(const socket::client&)>& non_sysop_filter();
        int relay_vote(datagram*, peer_t* exclude); //cosumes the datagram in all cases
        void relay_local_deltas(datagram*, peer_t* exclude);
        void relay_evidence(datagram*, peer_t* exclude);

        using b::send;
        void send(const local_deltas_t&);
        void send_vote(datagram*);

        engine::daemon_t& engine_daemon;
    };

}

