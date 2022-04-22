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
#include "rendezvous_t.h"
#include <us/gov/likely.h>
#include <us/gov/socket/peer_t.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/protocol.h>
#include <us/gov/socket/outbound/caller_daemon_t.h>

#define loglevel "gov/socket"
#define logclass "rendezvous_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::rendezvous_t;

const char* c::KO_3029 = "KO 3029.2 Timeout.";
const char* c::KO_20190 = "KO 20190 Backend returned a KO code: ";

c::rendezvous_t(caller_daemon_t& caller_daemon): caller_daemon(caller_daemon) {
}

c::~rendezvous_t() {
    for (auto& i: *this) {
        delete i.second;
    }
}

void c::flush() {
    finished = true;
    cv.notify_all();
}

bool c::arrived(datagram* d) {
    assert(d != nullptr);
    log("arrived", d->service);
    {
        unique_lock<mutex> lock(mx);
        auto seq = d->decode_sequence();
        auto i = find(seq);
        if (unlikely(i == end())) {
            log("no process is waiting for this dgram", "seq", seq);
            return false;
        }
        if (unlikely(i->second != nullptr)) {
            log("sequence conflict, same seq arrived twice", "seq", seq);
            delete i->second;
        }
        log("a process is waiting for this dgram", "seq", seq);
        i->second = d;
    }
    log("wake sendrecv up");
    cv.notify_all();
    return true;
}

pair<ko, datagram*> c::sendrecv(client& peer, datagram* d, string& remote_error) {
    unique_lock<mutex> lock(mx);
    auto seq = ++next_seq;
    d->encode_sequence(seq);
    emplace(seq, nullptr);
    log("sendrecv. sending dgram seq", seq);
    assert(seq == d->decode_sequence());
    auto ret = caller_daemon.send1(peer, d);
    if (unlikely(is_ko(ret))) {
        log("sent failed", ret);
        remote_error.clear();
        return make_pair(ret, nullptr);
    }
    auto t0 = peer.activity.load();
    iterator i;
    while(true) {
        log("waiting for", CFG_DGRAM_ROUNDTRIP_TIMEOUT_SECS, "seconds");
        chrono::system_clock::time_point deadline = chrono::system_clock::now() + chrono::seconds(CFG_DGRAM_ROUNDTRIP_TIMEOUT_SECS);
        cv.wait_until(lock, deadline, [&] {
                if (unlikely(finished)) return true;
                auto i = find(seq);
                if (i == end()) return true;
                if (i->second != nullptr) return true;
                return false;
            });
        log("sendrecv: wakeup");
        if (unlikely(finished)) {
            return make_pair<ko, datagram*>("KO 60593 Interrupted while waiting for a response.", nullptr);
        }
        i = find(seq);
        if (i != end() && i->second == nullptr) {
            auto t1 = peer.activity.load();
            if (t1 > t0) { //something arrived, not whole dgram, but something.
                log("partial chunk has arrived, give more time since it is not stalled.");
                t0 = t1;
                continue;
            }
        }
        break;
    }
    if (unlikely(i == end())) {
        auto r = "KO 30291 Cancelled.";
        log(r, "seq not found in rendezvous table. Cancelled. seq=", seq);
        remote_error.clear();
        return make_pair(r, nullptr);
    }
    auto dr = i->second;
    erase(i);
    if (dr == nullptr) {
        log(KO_3029, "null dgram found in rendezvous table. Timeout. seq=", seq);
        remote_error.clear();
        return make_pair(KO_3029, nullptr);
    }
    if (dr->service == gov::protocol::socket_error) {
        io::blob_reader_t reader(*dr);
        auto r = reader.read(remote_error);
        if (is_ko(r)) {
            remote_error = r;
        }
        log(KO_20190, remote_error);
        delete dr;
        return make_pair(KO_20190, nullptr);
    }
    else {
        remote_error.clear();
    }
    log("returning dgram svc", dr->service);
    return make_pair(ok, dr);
}

