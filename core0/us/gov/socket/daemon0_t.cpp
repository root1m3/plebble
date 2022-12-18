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
#include <fcntl.h>

#include <us/gov/io/blob_writer_t.h>
#include <us/gov/io/blob_reader_t.h>

#include "outbound/caller_daemon_t.h"
#include "types.h"
#include "daemon0_t.h"

#define loglevel "gov/socket"
#define logclass "daemon0_t"
#include "logs.inc"
#include <us/gov/socket/dto.inc>

using namespace us::gov::socket;
using c = us::gov::socket::daemon0_t;

c::daemon0_t(channel_t channel): channel(channel) {
    log("daemon0 constructor on channel", channel);
}

c::~daemon0_t() {
    log("waiting for shutdown tasks to complete");
    join();
}

void c::process_ok_work(peer_t& peer, datagram* d) {
    log("process_ok_work", peer.endpoint(), d->service);
    assert(d != nullptr);
    send1(peer, d);
}

bool c::process_unhandled(client& peer, datagram* d) {
    log("process_unhandled", d->service);
    return rendezvous.arrived(d);
}

string c::rewrite(ko r) const {
    if (is_ko(r)) return r;
    return "";
}

void c::process_ko_work(peer_t& peer, seq_t seq, ko r) {
    process_ko_work(peer, channel, seq, r);
}

void c::process_ko_work(peer_t& peer, channel_t channel, seq_t seq, ko r) {
    assert(is_ko(r));
    log("process_ko_work", r);
    string e = rewrite(r);
    log("process_ko_work (rewrite)", e);
    peer.send1(write_datagram(channel, protocol::socket_error, seq, e));
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "socket::daemon_t:" << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump(prefix, os);
}

