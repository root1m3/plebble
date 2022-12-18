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

#include <us/gov/likely.h>
#include <us/gov/socket/client.h>
#include <us/gov/socket/datagram.h>

#include "caller_daemon_t.h"

#define loglevel "gov/socket/outbound"
#define logclass "caller_daemon_t"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::caller_daemon_t;

c::caller_daemon_t(): rendezvous(*this) {
}

ko c::send1(client& peer, datagram* d0) {
    log("send1", d0, "svc", d0->service, "sz", d0->size());
    if (unlikely(d0 == nullptr)) {
        auto r = "KO 30291 Not sending null datagram.";
        log(r);
        return r;
    }
    if (unlikely(!is_active())) {
        delete d0;
        ko r = "KO 50481 sendq not active.";
        log(r);
        return r;
    }
    log("send1");
    if (unlikely(peer.sendref.load() < 0)) {
        delete d0;
        auto r = "KO 90162 Client waiting to die.";
        log(r);
        return r;
    }
    uint16_t pri = d0->decode_service();
    assert(api_v != 0); //top peer instance must set api_v
    if (peer.peer_api_v != api_v) { 
        if (peer.peer_api_v < api_v || peer.peer_api_v == 255) {
            auto svc = pri;
            peer.translate_svc(svc, false);
            log("Peer apiv is", peer.peer_api_v, ".Translating svc", pri, " -> ", svc);
            d0->encode_service(svc);
        }
    }
    {
        log("encrypting");
        pair<ko, datagram*> d = peer.encrypt0(d0);
        if (unlikely(is_ko(d.first))) {
            assert(d.second == nullptr);
            return d.first;
        }
        assert(d.second != nullptr);
        d0 = d.second;
    }
    log("delivering datagram to sendq. priority", pri);
    {
        using namespace chrono;
        peer.activity.store(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    }
    return b::send(d0, &peer, pri);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "Hello from socket::caller_daemon\n";
    os << "sendq daemon active:" << is_active() << '\n';
    b::dump(prefix, os);
}

ko c::start() {
    log("starting caller_daemon");
    return b::start();
}

void c::stop() {
    log("stop");
    b::stop();
    log("flushing rendezvous");
    rendezvous.flush();
}

