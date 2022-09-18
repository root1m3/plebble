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
#include "peer_t.h"
#include "daemon_t.h"
#include "protocol.h"
#include "net_daemon_t.h"
#include <us/gov/likely.h>
#include <us/gov/auth/peer_t.h>
#include <us/gov/peer/protocol.h>
#include <us/gov/engine/db_t.h>
#include "protocol.h"
#include "net_daemon_t.h"

#define loglevel "gov/engine"
#define logclass "peer_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

constexpr array<const char*, c::num_stages> c::stagestr;

c::peer_t(net_daemon_t& net_daemon, int sock): b(net_daemon, sock) {
}

c::~peer_t() {
}

bool c::process_work(datagram* d) {
    log("process_work", d->service);
    using namespace protocol;
    /// Assuming the following svc ordering:
    /// master definition: us/api/model.cpp
    static_assert(socket_end <= id_end);
    static_assert(id_end <= auth_end);
    static_assert(auth_end <= peer_end);
    static_assert(peer_end <= relay_end);
    static_assert(relay_end <= dfs_end);
    static_assert(dfs_end <= engine_end);
    static_assert(engine_end <= cash_end);
    static_assert(cash_end <= engine_auth_end);
    static_assert(engine_auth_end <= traders_end);
    static_assert(traders_end <= sys_end);
    using namespace us::gov::protocol;
    if (d->service < net_daemon_end) {
        log("processing net_daemon datagram", d->service);
        return b::process_work(d);
    }
    assert(d->service >= engine_begin);

    log("work svc > engine_begin", engine_begin);
    if (d->service < engine_end) {
        return process_work__engine(d);
    }
    if (d->service < cash_end) {
        return process_work__cash(d);
    }
    if (d->service < engine_auth_end) {
        return process_work__engine_auth(d);
    }
    if (d->service < traders_end) {
        return process_work__traders(d);
    }
    if (d->service < sys_end) {
        return process_work__sys(d);
    }
    return false;
}

ko c::authorizeX(const pub_t& p, pin_t pin) {
    log("authorize?", endpoint(), sock, p, pin);
    if (unlikely(engine_daemon().db->auth_app->node_pubkey != p)) {
        return ok; //Any pubkey is welcome in the public protocol
    }
    log("sysop pubkey->sysop_intent", (is_role_sysop() ? "yes" : "no"));
    if (unlikely(is_role_sysop())) {
        return ok;  //allow multiple sysop connections, reject self non-sysop connections
    }
    auto r = "KO 90483 non-sysop self connection";
    log(r);
    return r;
}

daemon_t& c::engine_daemon() {
    return static_cast<net_daemon_t&>(daemon).engine_daemon;
}

const daemon_t& c::engine_daemon() const {
    return static_cast<const net_daemon_t&>(daemon).engine_daemon;
}

void c::verification_completed(pport_t rpport, pin_t pin) {
    log("verification_completed", endpoint(), sock, "rpport", rpport, "pin", pin);
    b::verification_completed(rpport, pin);
    if (unlikely(!verification_is_fine())) {
        auto r = "KO 50342 verification_not_fine";
        log(r, "verification_not_fine->disconnect", endpoint());
        disconnect(0, r);
        return;
    }
    if (static_cast<gov::auth::peer_t*>(this)->stage != gov::auth::peer_t::authorized) {
        auto r = "KO 50091 Not authorized.";
        log(r, "disconnect-not authorized", endpoint());
        disconnect(0, r);
        return;
    }
    log("role", role, "=", rolestr[role]);
    if (is_role_peer() || is_role_sysop()) {
        engine_daemon().db->auth_app->basic_auth_completed(this, rpport);
        if (unlikely(stage == unknown)) {
            auto r = "KO 66506 Reached unknown stage.";
            log(r, "disconnect - stage set to unknown");
            gov::auth::peer_t::stage = gov::auth::peer_t::denied;
            disconnect(0, r);
            return;
        }
        if (unlikely(stage == sysop)) {
            log("sysop");
            if (engine_daemon().sysop_allowed) {
                gov::auth::peer_t::stage = gov::auth::peer_t::authorized;
            }
            else {
                auto r = "KO 30090 Sysop connections are not allowed.";
                log(r, "engine doesnt allow sysop connections (-ds)");
                gov::auth::peer_t::stage = gov::auth::peer_t::denied;
                disconnect(0, r);
                return;
            }
        }
        if (unlikely(gov::auth::peer_t::stage != gov::auth::peer_t::authorized)) { //looks like redundant, handled above
            auto r = "KO 32030 Not authorized.";
            log(r, "->disconnect");
            disconnect(0, r);
            return;
        }
        log("stage", stagestr[stage]);
        if (stage == node || stage == out || stage == hall || stage == sysop) {
//            if (!static_cast<gov::peer::daemon_t&>(daemon).grid.add(*this, true)) {
            if (!static_cast<gov::peer::daemon_t&>(daemon).clique.add(*this, true)) {
                auto r = "KO 10040 All lines are busy.";
                gov::auth::peer_t::stage = gov::auth::peer_t::denied;
                log(r, "disconnect-grid doesn't accept", endpoint(), sock);
                disconnect(0, r);
                return;
            }
            log("added to grid");
        }
        else {
            auto r = "KO 22010 Unknown stage.";
            log(r, "disconnect-unknown stage", stage);
            gov::auth::peer_t::stage = gov::auth::peer_t::denied;
            disconnect(0, r);
            return;
        }
    }
    else {
        if (is_role_device()) {
            log("device");
            stage = device;
            if (!static_cast<gov::peer::daemon_t&>(daemon).grid_dev.add(*this, false)) {
                auto r = "KO 50400 All lines are busy.";
                log(r, "disconnect-grid_dev doesn't accept", endpoint(), sock);
                disconnect(0, r);
                return;
            }
            log("added to grid-dev");
        }
        else {
            auto r = "KO 10500 Unknown role.";
            log(r, "disconnect-unknown role", role);
            gov::auth::peer_t::stage = gov::auth::peer_t::denied;
            disconnect(0, r);
            return;
        }
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "engine: stage " << stagestr[stage] << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

