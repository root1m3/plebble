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
#include "daemon_t.h"
#include <chrono>

#include <us/gov/io/cfg.h>
#include <us/gov/socket/client.h>

#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/peer"
#define logclass "daemon_trusted_ring"
#include "logs.inc"

using namespace us::gov::peer;
using c = us::gov::peer::daemon_t;

#ifdef CFG_TOPOLOGY_RING

void c::grid_rotate(nodes_t& nodes, mutex& nodes_mx) {
    using client = gov::socket::client;
    log("rotate-fixed-trusted-ring");
    host_t a;
    port_t p;
    {
        lock_guard<mutex> lock(grid.mx);
        assert(grid.size() == 2);
        log("pre (inbound)", &*pre, pre->first, client::ip4_decode(pre->second.net_address), pre->second.port);
        log("cur (outbound)", &*cur, cur->first, client::ip4_decode(cur->second.net_address), cur->second.port);
        int slotsfree = 0;
        for (auto i = grid.begin(); i != grid.end(); ++i) {
            if ((*i) == nullptr) ++slotsfree;
        }
        if (slotsfree == 0) return;
        for (auto i = grid.begin(); i != grid.end(); ++i) {
            if ((*i) != nullptr) {
                if (static_cast<id::peer_t*>(*i)->pubkey.hash() == cur->first) {
                    log("Outbound connection is already set");
                    return;
                }
            }
        }
        if (slotsfree == 0) {
            log("grid full");
            return;
        }
        log("connecting to", cur->first);
        a = cur->second.net_address;
        p = cur->second.port;
    }
    log("address port", a, client::endpoint(a, p));
    auto rc = grid_connect(make_pair(a, p), [](peer_t*) {}, [](peer_t*) {});
    if (is_ko(rc)) {
        log(rc);
    }
    return;
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "peer_daemon");
    #endif
    if (us::gov::socket::datagram::system_channel == 0) {
        log("us::gov::socket::datagram::system_channel must be non-zero to use an incompatible topology", us::gov::socket::datagram::system_channel);
        exit(1);
        return;
    }
    log("fixed-trusted ring configuration");
    log("ignoring edges parameter, using 2");
    edges = 2;
    pool.resize(10);

    log("using", homedir() + "/trusted_ring");
    vector<pair<hash_t, hostport_t>> n = us::gov::io::cfg::parse_nodes_file2(homedir() + "/trusted_ring");
    set_nodes(n);
    if (nodes.empty()) {
        log("Fatal error. Expected ring definition entry", homedir() + "/trusted_ring");
        cerr << "Fatal error. Expected ring definition at " << homedir() + "/trusted_ring" << endl;
        exit(1);
        return;
    }
    log("nodes grid sizes ", nodes.size(), grid.size());
    #if CFG_LOGS == 1
        log("RING:");
        for (auto i = nodes.begin(); i != nodes.end(); ++i) {
            log(i->first, gov::socket::client::ip4_decode(i->second.net_address), i->second.port);
        }
    #endif
    cur = nodes.end();
    pre = nodes.end();
    log("locating myself in node list");
    for (auto i = nodes.cbegin(); i != nodes.cend(); ++i) {
        if (i->first == myself) {
            cur = i;
            pre = i;
            ++cur;
            if (cur == nodes.end()) cur = nodes.begin();
            if (pre == nodes.begin()) advance(pre, nodes.size() - 1); else --pre;
            log("found myself. linking ring in->me->out.", pre->first, myself, cur->first);
            break;
        }
        log("is this me?", i->first, myself);
    }
    if (cur == nodes.end()) {
        log("cannot find myself in the ring. ", myself);
        cerr << "Fatal error. Cannot find myself in the ring" << endl;
        exit(1);
        return;
    }
    log("enter main loop");
    while(isup()) {
        log("pre", &pre, &*pre, pre->first, "cur", &cur, &*cur, cur->first);
        assert(cur != nodes.end());
        assert(pre != nodes.end());
        #if CFG_LOGS == 1
            log("RING:");
            for (auto i = nodes.begin(); i != nodes.end(); ++i) {
                log(i->first, gov::socket::client::ip4_decode(i->second.net_address), i->second.port);
            }
        #endif
        grid_rotate(nodes);
        log("pre", &pre, &*pre, pre->first, "cur", &cur, &*cur, cur->first);
        wait();
    }
    log("end");
}

#endif

