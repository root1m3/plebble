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
#include <random>

#include <us/gov/io/cfg.h>

#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/peer"
#define logclass "daemon_untrusted_mesh"
#include "logs.inc"

using namespace us::gov::peer;
using c = us::gov::peer::daemon_t;

#ifdef CFG_TOPOLOGY_MESH

void c::clique_rotate(nodes_t& nodes, mutex& nodes_mx) {
    for (auto grid: clique) {
        grid_rotate(nodes, nodes_mx, *grid);
    }
}

void c::grid_rotate(nodes_t& nodes, mutex& nodes_mx, grid_t& grid) {
    log("rotate");
    unique_lock<mutex> lock1(grid.mx_);
    bool is_full{false};
    {
        int full = 1 + grid.size() / 2;
        for (auto i = grid.begin(); i != grid.end(); ++i) {
            if (*i != nullptr) {
                if (--full == 0) {
                    is_full = true;
                    break;
                }
            }
        }
        if (is_full) {
            log("ok, grid is filled enough with outbound connections.", 1 + grid.size() / 2);
            lock1.unlock();
            return;
        }
    }
    log("grid has room. prepare next connection");
    unique_lock<mutex> lock2(nodes_mx);
    log("nodes, grid sizes ", nodes.size(), grid.size());
    logdump("nodes>", nodes);
    if (nodes.empty()) {
        log("nodes is empty");
        lock2.unlock();
        lock1.unlock();
        return;
    }
    peer_t* cli = nullptr;
    int tries = nodes.size();
    while(cli == nullptr && tries-- > 0 && t::isup()) {
        log("tries", tries);
        static mt19937_64 rng(random_device{}());
        uniform_int_distribution<> d(0, nodes.size() - 1);
        auto cur = nodes.begin();
        assert(cur != nodes.end());
        auto dist = d(rng);
        advance(cur, dist);
        log("nodes cur", dist, nodes.size());
        assert(dist < nodes.size());
        auto cur0 = cur;
        assert(!myself.is_zero());
        while(true) {
            assert(!cur->first.is_zero());
            log("cur", cur->first, "myself pubkeyh", myself);
            if (cur->first == myself) {
                log("skipped myself", myself);
                ++cur;
                if (cur == nodes.end()) {
                    cur = nodes.begin();
                }
                if (cur == cur0) {
                    log("no more nodes left to connect");
                    lock2.unlock();
                    lock1.unlock();
                    return;
                }
                continue;
            }
            if (!grid.find(cur->first)) {
                log("found suitable peer to connect to", cur->first);
                break;
            }
            log(cur->first, "already in grid");
            ++cur;
            if (cur == nodes.end()) {
                cur = nodes.begin();
            }
            if (cur == cur0) {
                log("no more nodes left to connect");
                lock2.unlock();
                lock1.unlock();
                return;
            }
        }
        host_t a = cur->second.net_address;
        assert(a != 0);
        port_t p = cur->second.port;
        log("connecting to", socket::client::ip4_decode(a), p, cur->first);
        lock2.unlock();
        lock1.unlock();
        auto rc = grid_connect(make_pair(a, p), [](peer_t*) {},
            [&](peer_t* p) {
                log("pre-attach");
                cli = p;
            });
        if (is_ko(rc)) {
            log(rc);
        }
        log("on-going connection created?", cli != nullptr ? "yes" : "no");
        lock1.lock();
        lock2.lock();
    }
    log("end rotate");
    lock2.unlock();
    lock1.unlock();
    return;
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "peer_daemon");
    #endif
    log("open-untrusted mesh configuration");
    if (unlikely((*clique.begin())->size() == 0)) {
        log("End. edges 0");
        return;
    }
    log("grid size", (*clique.begin())->size(), "grid_dev size", grid_dev.size());
    if (wait_rnd_before_start()) { //distribute node activity in time
        mt19937_64 rng(random_device{}());
        uniform_int_distribution<> d(0, 60); //todo cycle time
        int s = d(rng);
        log("peerd-sleep random seconds", s);
        mutex mx;
        unique_lock<mutex> lock(mx);
        cv.wait_for(lock, chrono::seconds(s), [&]{ return t::resumed(); });
        t::reset_wait();
    }
    log("enter main loop");
    while(t::isup()) { //don't lock nodes just to check if is empty, in the unlikely case it returns a garbage boolean it will be through the wrong branch once in a million times with the effect of connecting to a node listed in seeds instead og the current nodeset
        log("switch to db nodes", "force_seeds", force_seeds, "nodes empty?", nodes.empty());
        while(t::isup()) {
            if (nodes.empty() || force_seeds) break;
            clique_rotate(nodes, mx_nodes);
            wait();
        }
        log("switch to seed nodes");
        while(t::isup()) {
            if (!force_seeds && !nodes.empty()) break;
            clique_rotate(seed_nodes, mx_seed_nodes);
            wait();
        }
    }
    log("end");
}

#endif

