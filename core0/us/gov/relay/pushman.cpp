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
#include "pushman.h"
#include <us/gov/socket/datagram.h>
#include <us/gov/io/blob_writer_t.h>

#include "peer_t.h"

#define loglevel "gov/relay"
#define logclass "pushman"
#include "logs.inc"

using namespace us::gov::relay;
using c = us::gov::relay::pushman;

c::pushman(daemon_t& daemon): daemon(daemon), t(bind(&c::run, this), bind(&c::onwakeup, this)) {
    vpush = new vector<push_item>;
    vpush->reserve(10);
}

c::~pushman() {
    delete vpush;
}

void c::onwakeup() {
    cv.notify_all();
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, logfile);
    #endif
    while(isup()) {
        push_to_devices();
        wait();
    }
    log("end");
}

void c::flush_push() {
    log("flush_push");
    task_wakeup();
}

void c::schedule_push(datagram* d, const filter_t& filter) {
    if (unlikely(d == nullptr)) {
        log("schedule_push. Ignored null datagram");
        return;
    }
    {
        unique_lock<mutex> lock(mx);
        vpush->emplace_back(push_item(d, filter));
    }
    flush_push();
}

void c::push_to_devices() {
    log("push_to_devices");
    vector<push_item>* v;
    {
        unique_lock<mutex> lock(mx);
        if (vpush->empty()) {
            return;
        }
        v = vpush;
        vpush = new vector<push_item>;
        vpush->reserve(10);

    }
    daemon.grid_devices_send(*v);
    for (auto& i: *v) {
        delete i.dgram;
    }
    delete v;
}

void c::wait() {
    log("wait");
    if (reset_wait()) {
        log("no wait");
        return;
    }
    {
        unique_lock<mutex> lock(mx);
        log("waiting");
        cv.wait(lock, [&]{ return resumed(); });
    }
    reset_wait();
    log("resumed");
}

ko c::push_KO(const hash_t& tid, ko msg, const filter_t& filter) {
    log("pushing KO:", msg, "tid", tid);
    assert(msg != ok);
    string m = msg;
    auto blob = io::blob_writer_t::make_blob(m);
    schedule_push(peer_t::push_in_t::get_datagram(daemon.channel, 0, tid, push_ko, blob), filter);
    return msg;
}

ko c::push_OK(const hash_t& tid, const string& msg, const filter_t& filter) {
    log("pushing OK:", msg, "tid", tid);
    auto blob = io::blob_writer_t::make_blob(msg);
    schedule_push(peer_t::push_in_t::get_datagram(daemon.channel, 0, tid, push_ok, blob), filter);
    return ok;
}

ko c::push_KO(ko msg, const filter_t& filter) {
    log("pushing KO:", msg);
    assert(msg != ok);
    string m = msg;
    auto blob = io::blob_writer_t::make_blob(m);
    schedule_push(peer_t::push_in_t::get_datagram(daemon.channel, 0, hash_t(0), push_ko, blob), filter);
    return msg;
}

ko c::push_OK(const string& msg, const filter_t& filter) {
    log("pushing OK:", msg);
    auto blob = io::blob_writer_t::make_blob(msg);
    schedule_push(peer_t::push_in_t::get_datagram(daemon.channel, 0, hash_t(0), push_ok, blob), filter);
    return ok;
}

