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
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/id"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::id;
using c = us::gov::id::daemon_t;

c::daemon_t(channel_t channel, port_t port, pport_t pport, int workers): b(channel, port, workers), pport(pport) {
    log("set pport", pport);
}

socket::client* c::create_client(sock_t sock) {
    log("create_client", sock);
    return new peer_t(*this, sock);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "id::daemon_t: This node public key: " << get_keys().pub << " addr " << get_keys().pub.hash() << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

void c::disconnect(const pub_t& p, seq_t seq, const reason_t& reason) {
    log("disconnect pubkey", p);
    unique_lock<mutex> lock(peers.mx);
    for (auto& i: peers) {
        if (p == static_cast<peer_t*>(i.second)->pubkey) {
            i.second->disconnect(seq, reason);
        }
    }
}

bool c::is_duplicate(const pub_t& p) const {
    log("is_duplicate?", p);
    unique_lock<mutex> lock(peers.mx);
    int count{0};
    for (auto& i: peers) {
        if (p == static_cast<peer_t*>(i.second)->pubkey) {
            ++count;
            log("found",count);
            if(count > 1) {
                log("yes, there are at least 2 clients with pubk", p);
                return true;
            }
        }
    }
    log("not duplicate");
    return false;
}

