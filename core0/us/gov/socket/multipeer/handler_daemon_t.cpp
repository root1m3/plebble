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
#include "handler_daemon_t.h"

#include <chrono>
#include <sstream>

#include <us/gov/socket/protocol.h>
#include <us/gov/socket/peer_t.h>
#include <us/gov/socket/daemon_t.h>

#define loglevel "gov/socket/multipeer"
#define logclass "handler_daemon_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::multipeer::handler_daemon_t;

const char* c::KO_42093 = "KO 42093 wrong service.";

c::handler_daemon_t(daemon_t& daemon, port_t port, uint8_t workers): b(daemon, port), num_workers(workers) {
    log("constructor");
}

c::~handler_daemon_t() {
    log("destructor");
    join();
}

us::ko c::start() {
    log("start", "num_workers", num_workers);
    auto r = b::start();
    if (unlikely(r != ok)) {
        return r;
    }
    if (likely(num_workers > 0)) {
        #if CFG_LOGS == 1
            pool.logdir = logdir + "/thpool";
        #endif
        log("starting pool with num workers", num_workers);
        r = pool.start(num_workers);
        if (unlikely(r != ok)) {
            b::stop();
            return r;
        }
    }
    else {
        log("WARNING", "num_workers is zero");
    }
    return ok;
}

us::ko c::wait_ready(const time_point& deadline) const {
    ko r = b::wait_ready(deadline);
    if (unlikely(r != ok)) {
        return r;
    }
    if (likely(num_workers > 0)) {
        r = pool.wait_ready(deadline);
        if (unlikely(r != ok)) {
            return r;
        }
    }
    return ok;
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::stop() {
    log("stop");
    b::stop();
    log("stopping worker thread pool");
    pool.stop();
}

void c::join() {
    b::join();
    log("waiting for worker thread pool");
    pool.join();
    log("stopped");
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Hello from socket::daemon\n";
    os << prefix << "Num workers: " << +num_workers << '\n';
    pool.dump(prefix + "  ", os);
}

void c::receive_and_process(client* peer) {
    log("receive_and_process");
    pool.process(static_cast<peer_t*>(peer));
}

void c::set_busy_handler(busyled_t::handler_t* h) {
    busyled.set_handler(h);
}

bool c::process_unhandled(client& peer, datagram* d) {
    auto seq = d->decode_sequence();
    log("disconnect-unknown service", d->service, "dgram sz", d->size(), "seq", seq, "from", peer.endpoint());
    delete d;
    log("disconnect");
    peer.disconnect(seq, KO_42093);
    #if CFG_COUNTERS == 1
        ++counters.disconnection_unknown_service;
    #endif
    return true;
}

