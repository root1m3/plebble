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
#include <chrono>
#include <cassert>

#include <us/gov/config.h>
#include <us/gov/types.h>
#include <us/gov/auth/daemon_t.h>

#include "nodes_t.h"
#include "grid_t.h"
#include "types.h"

namespace us { namespace gov { namespace peer {

    struct daemon_t;

    struct mezzanine: bgtask {
        using b = bgtask;
        mezzanine(daemon_t*);
        daemon_t* d;
    };

    struct daemon_t: base_ns::daemon_t, peer::mezzanine {
        using b = base_ns::daemon_t;
        using t = peer::mezzanine;

        daemon_t(channel_t);
        daemon_t(channel_t, port_t port, pport_t pport, uint8_t edges, uint8_t devices, uint8_t workers);
        ~daemon_t() override;

        void dump(const string& prefix, ostream&) const;
        void visit(const function<void(peer_t&)>& visitor);
        vector<hostport_t> list_neighbours() const;
        vector<hostport_t> list_dev_neighbours() const;
        void purge_idle();

        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;
        void stop();
        void join();
        bool is_active() const { return b::is_active() && t::is_active(); }
        inline bool isup() const { return b::isup() && t::isup(); }

        inline void wakeup_grids() { t::task_wakeup(); }
        void wait();
        void dump_random_nodes(size_t num, ostream&) const;
        void on_destroy_(socket::client&) override;
        virtual void remote_port(const pub_t&, port_t) {};
        void test_connectivity();

        #ifdef CFG_TOPOLOGY_MESH
            virtual bool wait_rnd_before_start() const { return true; }
        #endif
        #ifdef CFG_TOPOLOGY_RING
            nodes_t::const_iterator pre, cur;
            virtual string homedir() const = 0;
        #endif

        //------ ring/mesh implementations
        void grid_rotate(nodes_t&, mutex&);
        void run();
        void on_t_wakeup();
        //-/---- ring/mesh implementations

        void set_seed_nodes(const vector<hostport_t>&);
        ko add_seed_node(const hostport_t&);
        void set_nodes(const vector<pair<hash_t, hostport_t>>&);
        void grid_setcur();
        void disconnect_one();
        ko grid_connect(const hostport_t& ipport, function<void(peer::peer_t*)> pre_connect, function<void(peer::peer_t*)> pre_attach);
        bool grid_connect_test(peer::peer_t*, const hostport_t&, ostream&);
        socket::client* create_client(sock_t sock) override { assert(false); return nullptr; }

    private:
        ko add_seed_node__(const hostport_t&);
        ko add_node__(const pair<hash_t, hostport_t>&);

    public:
        nodes_t seed_nodes;
        mutable mutex mx_seed_nodes;

        nodes_t nodes;
        mutable mutex mx_nodes;

        nodes_t hall;
        mutable mutex mx_hall;

        uint8_t edges;
        grid_t grid;


    public:
        condition_variable cv;
        hash_t myself;
        chrono::system_clock::time_point tlo;
        bool force_seeds{false};
        grid_t grid_dev;
    };

}}}


