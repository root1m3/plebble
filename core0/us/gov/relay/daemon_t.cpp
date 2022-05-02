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
#include <limits>
#include "peer_t.h"

#define loglevel "gov/relay"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::relay;
using c = us::gov::relay::daemon_t;

c::daemon_t(channel_t channel): b(channel) {
}

c::daemon_t(channel_t channel, port_t port, pport_t pport, uint8_t edges, uint8_t devices, int workers): b(channel, port, pport, edges, devices, workers) {
}

c::~daemon_t() {
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Hello from relay::daemon.\n";
}

int c::send(int num, const peer_t* exclude, datagram* d) {
    int n = send(num, exclude, *d);
    delete d;
    return n;
}

bool c::check_relay_permission(const hash_t&) const {
    return true;
}

int c::send(int num, const peer_t* exclude, const datagram& d) {
    if (num == 0) num = numeric_limits<int>::max();
    log("send neighbours", num);
    int succ = 0;
    unordered_set<const peer::peer_t*> visited;
    if (exclude != nullptr) visited.emplace(exclude);
    while(true) {
        if (succ == num) {
            log("done enough");
            break;
        }
        auto p = grid.prepare_worker_send(visited);
        if (p == nullptr) {
            break;
        }
        #ifdef CFG_PERMISSIONED_NETWORK
            if (!check_relay_permission(static_cast<id::peer_t*>(p)->pubkey.hash())) {
                log("lack of relay permissions", d.service, p->endpoint());
                continue;
            }
        #endif
        if (!static_cast<id::peer_t*>(p)->is_role_peer()) {
            log("not role peer", static_cast<id::peer_t*>(p)->rolestr[static_cast<id::peer_t*>(p)->role]);
            continue;
        }
        log("sending datagram to", d.service, p->endpoint());
        auto r = p->send1(new datagram(d));
        if (likely(r == ok)) {
            ++succ;
            log("success sending datagram svc", d.service, "to", p->endpoint());
        }
    }
    log("Sent succ", succ);
    return succ;
}

int c::grid_devices_send(const vector<push_item>& v) {
    log("grid_devices_send", v.size());
    unordered_set<const peer::peer_t*> visited;
    int succ = 0;
    while(true) {
        auto p = grid_dev.prepare_worker_send(visited);
        if (p == nullptr) {
            break;
        }
        log("sending datagram to", p->endpoint());
        for (auto& i: v) {
            if (!i.filter(static_cast<peer_t&>(*p))) {
                log("peer filtered out");
                continue;
            }
            auto r = p->send1(new datagram(*i.dgram));
            log(r == ok ? "success" : r);
            if (likely(r == ok)) {
                ++succ;
                log("success sending datagram to", p->endpoint());
            }
        }
    }
    log("Sent", v.size(), "dgrams succ to a number of devices=", succ);
    return succ;
}

int c::grid_devices_send(const datagram& d) {
    log("grid_devices_send");
    unordered_set<const peer::peer_t*> visited;
    int succ = 0;
    while(true) {
        auto p = grid_dev.prepare_worker_send(visited);
        if (p == nullptr) {
            break;
        }
        log("sending datagram to", d.service, p->endpoint());
        auto r = p->send1(new datagram(d));
        log(r == ok ? "success" : r);
        if (r == ok) {
            ++succ;
            log("success sending datagram to", p->endpoint());
        }
    }
    log("Sent succ to a number of devices", succ);
    return succ;
}

