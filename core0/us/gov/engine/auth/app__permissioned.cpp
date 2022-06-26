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
#ifdef CFG_PERMISSIONED_NETWORK

#include "app.h"
#include "acl_tx.h"
#include "local_delta.h"
#include "types.h"

#define loglevel "gov/engine/auth"
#define logclass "app__permissioned"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::app;

pub_t c::node_master_pubkey(NODEMASTER_PUBKEY);

bool c::account_state(const nodes_t& s, const hash_t& pubkeyh, host_t& netaddr, port_t& port) const {
    log("account_state from container");
    auto p = s.find(pubkeyh);
    if (likely(p == s.end())) return false;
    netaddr = p->second.net_address;
    port = p->second.port;
    log("found.", "netaddr", netaddr, "port", port);
    return true;
}

bool c::account_state(const auth::local_delta& w, const hash_t& pubkeyh, host_t& netaddr, port_t& port, peer_t::stage_t& stage) const {
    log("account_state from local_delta. nodes?");
    if (account_state(w.online, pubkeyh, netaddr, port)) {
        stage = peer_t::node;
        log("yes", "netaddr", netaddr, "port", port);
        return true;
    }
    log("account_state not found");
    return false;
}

bool c::account_state(const hash_t& pubkeyh, host_t& netaddr, port_t& port, peer_t::stage_t& stage) const {
    log("account_state from pubkeyh");
    log("acc state", "pool?");
    if (account_state(*pool, pubkeyh, netaddr, port, stage)) return true;
    log("acc state", "nodes?"); //TODO check if we need lock db
    if (account_state(db.nodes, pubkeyh, netaddr, port)) {
        stage = peer_t::node;
        log("yes", "netaddr", netaddr, "port", port);
        return true;
    }
    log("acc state", 3);
    log("acc state", "hall?");
    if (account_state(db.hall, pubkeyh, netaddr, port)) {
        stage = peer_t::hall;
        log("yes", "netaddr", netaddr, "port", port);
        return true;
    }
    log("acc state", "not found");
    return false;
}

bool c::process(const acl_tx& t) {
    log("process acl_tx evidence", t.addr, t.allow);
    if (t.pubkey != node_master_pubkey) {
        log("KO 40931 acl_tx: Invalid access.");
        return false;
    }
    ostream nullos(0);
    if (!t.verify(nullos)) {
        log("KO 40932 acl_tx: Invalid access. Failed verification");
        return false;
    }
    if (unlikely(t.addr.is_zero())) {
        log("KO 40933 acl_tx: Invalid address.");
        return false;
    }
    log("proc acl_tx", 1);
    account_t acc;
    peer_t::stage_t stage;
    peer_t::stage_t tstage = t.allow ? peer_t::node : peer_t::out;
    lock_guard<mutex> lock(mx_pool);
    if (account_state(t.addr, acc.net_address, acc.port, stage)) {
        log ("got account_state", acc.net_address, acc.port, peer_t::stagestr[stage]);
        if (stage == peer_t::node && tstage == peer_t::out) {
            log("proc acl_tx", 2);
            auto i = pool->online.find(t.addr);
            if (i != pool->online.end()) {
                log("proc acl_tx", 3);
                i->second.net_address = 0;
            }
            else {
                log("proc acl_tx", 4);
                acc.net_address = 0;
                pool->online.emplace(t.addr, acc);
            }
            return true;
        }
        else if (stage == peer_t::hall && tstage == peer_t::out) {
            log("proc acl_tx", 5);
            return true;
        }
        else if (stage == peer_t::hall && tstage == peer_t::node) {
            log("proc acl_tx", 8);
            auto i = pool->online.find(t.addr);
            if (i != pool->online.end()) {
                log("unexpected acl_tx", 10);
                i->second = acc;
            }
            else {
                log("proc acl_tx", 11);
                pool->online.emplace(t.addr, acc);
            }
            return true;
        }
        else {
            log("proc acl_tx", 13);
            return false;
        }
    }
    else {
        log("proc acl_tx", 14);
        return false;
    }
}

#endif

