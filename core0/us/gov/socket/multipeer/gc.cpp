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
#include <chrono>
#include <us/gov/socket/client.h>
#include "gc.h"

#define loglevel "gov/socket/multipeer"
#define logclass "gc_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::multipeer::gc_t;

c::gc_t(): t(bind(&c::onwakeup, this)) {
}

c::~gc_t() {
    join();
    for (auto& i: *this) delete i;
}

void c::add_(client* peer) {
    assert(peer->is_finished());
    {
        lock_guard<mutex> lock(mx);
        emplace(peer);
    }
    task_wakeup();
    log("client entered garbage collector", peer);
}

void c::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    if (empty()) os << "empty\n";
    for (auto& i: *this) {
        i->dump("", os);
        os << '\n';
    }
}

void c::onwakeup() {
    cv.notify_one();
}

void c::collect() {
    using namespace chrono_literals;
    using namespace chrono;
    unique_lock<mutex> lock(mx);
    if (!empty()) {
        log("gc: just wait a minimum to avoid fast loop, even though is not empty. not waiting for long though, there is something to delete.",size());
        cv.wait_for(lock, 3s, [&]{ return resumed(); });
    }
    else {
        log("gc: waiting with to=60 secs");
        cv.wait_for(lock, 60s, [&]{ return !empty() || resumed(); });
    }
    log("gc: wake up");
}

void c::task_init() {
    #if CFG_LOGS == 1
        log_start(logdir, "gc");
    #endif
}

void c::task_cleanup() {
    log("end");
}

void c::task() {
    unique_lock<mutex> lock(mx);
    for (iterator i = begin(); i != end();) {
        if ((*i)->busyrecv.load() == 0) {
            if ((*i)->sendref.load() <= 0) {
                log("deleting", *i);
                delete *i;
                i = erase(i);
            }
            else {
                log("skipped busy client (datagrams pending to be destroyed)");
                ++i;
                continue;
            }
        }
        else {
            log("skipped busy_worker (mx_recv is locked)");
            ++i;
            continue;
        }
    }
}

