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
#include <us/gov/config.h>

#include "daemon_t.h"
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/peer"
#define logclass "peer_t"
#include "logs.inc"
#include <us/gov/socket/dto.inc>

using namespace us::gov::peer;
using c = us::gov::peer::peer_t;

constexpr array<const char*, c::num_stages> c::stagestr;

chrono::seconds c::auth_window{socket::peer_t::stall + 5s};

const char* c::finished_reason_1 = "auth not granted in the allocated time.";

c::peer_t(daemon_t& daemon, sock_t sock): b(daemon, sock), stage(stage_service) {
    log("constructor");
    if (sock <= 0) {
        stage = stage_disconnected;
    }
}

c::~peer_t() {
    log("destructor");
}

ko c::connect(const hostport_t& hostport, pport_t pport, pin_t pin, role_t role, const request_data_t& request_data, bool block) {
    log("connecting_to", client::endpoint(hostport), "pport", pport, "pin", pin, "role", rolestr[role], "request_data", request_data);
    auto r = b::connect(hostport, pport, pin, role, request_data, block);
    if (likely(r == ok)) {
        log("OK", "fd is", sock);
        stage = stage_service;
    }
    else {
        stage = stage_disconnected;
    }
    return r;
}

ko c::authorize(const pub_t& p, pin_t, request_data_t& request_data) {
    log("authorize", p, request_data);
    #ifdef CFG_TOPOLOGY_RING
        const daemon_t& d = static_cast<daemon_t&>(daemon);
        if (d.pre == d.nodes.end() || d.cur == d.nodes.end()) {
            auto r = "KO 49984 Not ready";
            log(r);
            disconnect(0, "Not ready");
            return r;
        }
        if (d.pre->first != p.hash() && d.cur->first != p.hash()) {
            auto r = "KO 49884 unexpected node";
            log(r, p.hash(), d.pre->first, d.cur->first);
            disconnect(0, r);
            return r;
        }
        return ok;
    #else
        auto r = "KO 50958 auth is denied by default policy";
        log(r);
        return r;
    #endif
}

bool c::check_auth_not_granted() const {
    log("check_auth_not_granted");
    using namespace std::chrono;
    if (likely(static_cast<const auth::peer_t*>(this)->stage == auth::peer_t::authorized)) return false;
    auto d = system_clock::now() - since;
    log("auth_window", duration_cast<seconds>(d).count(), "/", duration_cast<seconds>(auth_window).count());
    if (d >= auth_window) {
        log("time for auth elapsed", "d(sec)=", duration_cast<seconds>(d).count(), "auth_window (sec)=", duration_cast<seconds>(auth_window).count());
        return true;
    }
    log("auth not completed yet.", auth::peer_t::stagestr[static_cast<const auth::peer_t*>(this)->stage]);
    return false;
}

bool c::check_idle() const {
log("RBF IDLE disabled");
return false;
    using namespace std::chrono;
    using namespace chrono_literals;
    auto n = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    log("check_idle", endpoint(), pubkey, n, activity.load(), n-activity.load(), ">=", maxidle_ms, (n-activity.load() >= maxidle_ms ? "IDLE" : ""));
    if (n-activity.load() >= maxidle_ms) {
        log("IDLE");
        return true;
    }
    return false;
}

pair<c::security_level_t, string> c::security_level() const {
    auto sl = b::security_level();
    if (unlikely(sl.first != security_level_ok)) {
        return move(sl);
    }
    assert(sl.first == security_level_ok);

    if (check_auth_not_granted()) {
        log("auth_not_granted");
        return make_pair(security_level_disconnect, finished_reason_1); //queries without auth are disconnected after inactivity
    }
    if (check_idle()) {
        log("idle");
        return make_pair(security_level_disconnect, "idle"); //queries without auth are disconnected after inactivity
    }

    return make_pair(security_level_ok, "");
}

void c::disconnectx(channel_t channel, seq_t seq, const reason_t& reason) {
    log("disconnect", endpoint(), "ch", channel, seq, reason);
    stage = stage_disconnecting;
    b::disconnectx(channel, seq, reason);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "  peer: stage " << stagestr[stage] << '\n'; //" age " << age(since) << " idle " << age(activity) << endl;
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

bool c::process_work(datagram* d) {
    assert(d != nullptr);
    #ifdef has_us_gov_peer_api
    using namespace us::gov::protocol;
    switch(d->service) {
        #include <us/api/generated/gov/c++/peer/hdlr_svc-router>
    }
    #endif
    return b::process_work(d);
}

#ifdef has_us_gov_peer_api
#include <us/api/generated/gov/c++/peer/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/peer/hdlr_local-impl

ko c::handle_f1() {
    log("f1");
    // Implement here.

    return ok;
}

//-/----------------apitool - End of API implementation.

#include <us/api/generated/gov/c++/peer/cllr_rpc-impl>
#endif

