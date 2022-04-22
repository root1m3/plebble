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
#include "dialogue_c_t.h"

#include <us/gov/crypto/types.h>
#include <us/gov/socket/datagram.h>

#include <us/wallet/protocol.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#include "c_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "dialogue_c_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::bootstrap::dialogue_c_t;

c::dialogue_c_t(bootstrapper_t& parent): parent(parent) {
    log("dialogue_c constructor", &parent);
}

c::~dialogue_c_t() {
    log("dialogue_c destructor");
    delete state;
}

c::state_t* c::release_() {
    log("release_a", &parent);
    auto x = state;
    state = nullptr;
    return x;
}

void c::reset() {
    log("dialogue_c_t::reset", &parent);
    delete state;
    state = nullptr;
}

ko c::initiate(peer_t& peer, const string& wloc, const protocol_selection_t& protocol_selection) {
    log("dialogue_c_t::initiate", "TRACE 8c", protocol_selection.first, protocol_selection.second);
    unique_lock<mutex> lock(mx);
    log("mx locked");
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    state = new state_t();
    auto ps = protocol_selection;
    auto p = parent.trader->parent.create_protocol(move(ps));
    if (p.first != ok) {
        log(p.first);
        assert(p.second == nullptr);
        reset();
        return move(p.first);
    }
    ch_t ch(0);
    auto r = parent.trader->set_protocol(p.second, ch);
    if (is_ko(r)) {
        reset();
        return r;
    }
    lock.unlock();
    blob_t blob;
    {
        assert(parent.trader->w != nullptr);
        lock_guard<mutex> lock(parent.trader->mx);
        c1_t x(parent.trader->w->local_endpoint, wloc, parent.trader->opposite_protocol_selection(), parent.trader->shared_params(), parent.trader->my_challenge);
        x.write(blob);
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, trader_t::svc_handshake_c1, blob));
}

ko c::handshake(peer_t& peer, c1_t&& o) {
    log("dialogue_c_t::handshake_1", "seq = 0");
    if (!parent.trader->parent.daemon.has_home(o.wloc)) {
        auto r = "KO 80795 Invalid wallet.";
        log(r);
        return r;
    }
    unique_lock<mutex> lock(mx);
    log("mx locked a");
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    auto r = parent.on_c(1);
    if (is_ko(r)) {
        return r;
    }
    state = new state_t();
    ++state->seq;
    lock.unlock();
    auto w = parent.trader->parent.daemon.users.get_wallet(o.wloc);
    parent.trader->init(parent.trade_id, o.endpoint, *w);
    parent.trader->online(peer);
    ch_t ch(0);
    {
        auto r = parent.trader->deliver(move(o.challenge), move(o.protocol_selection), move(o.params), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    log("ch", ch.to_string());
    return update_peer2(peer, move(ch));
}

ko c::handshake(peer_t& peer, c2_t&& o) {
    log("dialogue_c_t::handshake_2", &parent);
    unique_lock<mutex> lock(mx);
    log("mx locked");
    if (state == nullptr) {
        auto r = KO_43344;
        log(r);
        reset();
        return r;
    }
    ++state->seq;
    auto r = parent.on_c(2);
    if (is_ko(r)) {
        return r;
    }
    if (--state->nc2 < 0) { //c1 should only arrive once
        auto r = KO_43344;
        log(r);
        reset();
        return r;
    }
    lock.unlock();
    ch_t ch(0);
    {
        log("c2 brought personality", o.personality_proof.raw().to_string());
        auto r = parent.trader->deliver(move(o.endpoint), move(o.challenge), move(o.personality_proof), move(o.params), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    if (state->seq == 1) { //remote doesn't have our stuff
        log("fully update peer first time even though we don't register changes from the las peer's update");
        ch.shared_params_changed = ch.personality_changed = true;
    }
    return update_peer(peer, move(ch));
}

ko c::handshake(peer_t& peer, c3_t&& o) {
    log("dialogue_c_t::handshake_3", &parent);
    unique_lock<mutex> lock(mx);
    log("mx locked");
    if (state == nullptr) {
        auto r = KO_43344;
        log(r, state->seq);
        reset();
        return r;
    }
    ++state->seq;
    auto r = parent.on_c(3);
    if (is_ko(r)) {
        return r;
    }
    if (--state->nc3_4 < 0) {
        auto r = KO_43344;
        log(r);
        reset();
        return r;
    }
    lock.unlock();
    ch_t ch(0);
    {
        log("c3 brought personality", o.personality_proof.raw().to_string());
        auto r = parent.trader->deliver(move(o.personality_proof), move(o.params), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    return update_peer(peer, move(ch));
}

ko c::handshake(peer_t& peer, c4_t&& o) {
    log("dialogue_c_t::handshake_4", &parent);
    unique_lock<mutex> lock(mx);
    log("mx locked");
    if (state == nullptr) {
        auto r = KO_43344;
        log(r, state->seq);
        reset();
        return r;
    }
    ++state->seq;
    auto r = parent.on_c(4);
    if (is_ko(r)) {
        return r;
    }
    if (--state->nc3_4 < 0) {
        auto r = KO_43344;
        log(r);
        reset();
        return r;
    }
    lock.unlock();
    ch_t ch(0);
    {
        auto r = parent.trader->deliver(move(o.params), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    return update_peer(peer, move(ch));
}

ko c::update_peer(peer_t& peer, ch_t&& ch) {
    log("update_peer", ch.to_string());
    assert(ch.closed());
    svc_t svc;
    blob_t blob;
    if (ch.personality_changed && ch.shared_params_changed) {
        log("dialogue_c. update_peer - personality_changed", "shared_params_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        assert(parent.trader->peer_challenge.is_not_zero());
        c3_t x(parent.trader->my_personality.gen_proof(parent.trader->peer_challenge), parent.trader->shared_params());
        x.write(blob);
        svc = trader_t::svc_handshake_c3;
    }
    else if (ch.personality_changed) {
        log("dialogue_c. update_peer - personality_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        assert(parent.trader->peer_challenge.is_not_zero());
        c3_t x(parent.trader->my_personality.gen_proof(parent.trader->peer_challenge));
        x.write(blob);
        svc = trader_t::svc_handshake_c3;
    }
    else if (ch.shared_params_changed) {
        log("dialogue_c. update_peer - shared_params_changed");
        lock_guard<mutex> lock(parent.trader->mx);
        c4_t x(parent.trader->shared_params());
        x.write(blob);
        svc = trader_t::svc_handshake_c4;
    }
    else {
        log("dialogue_c. update_peer - no changes.");
        svc = 0;
    }
    if (ch.need_update_devices()) {
        log("push_data");
        parent.trader->schedule_push_data(peer.get_lang());
    }
    if (svc == 0) {
        lock_guard<mutex> lock(mx);
        reset();
        return ok;
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, svc, blob));
}

ko c::update_peer2(peer_t& peer, ch_t&& ch) {
    log("update_peer", ch.to_string());
    assert(ch.closed());
    svc_t svc;
    blob_t blob;
    if (true || (ch.personality_changed && ch.shared_params_changed)) {
        log("dialogue_c. update_peer - personality_changed", "shared_params_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        assert(parent.trader->w != nullptr);
        lock_guard<mutex> lock(parent.trader->mx);
        c2_t x(parent.trader->w->local_endpoint, parent.trader->shared_params(), parent.trader->my_personality.gen_proof(parent.trader->peer_challenge), parent.trader->my_challenge);
        x.write(blob);
        svc = trader_t::svc_handshake_c2;
    }
    else {
        log("dialogue_c. update_peer - no changes.");
        svc = 0;
    }
    if (ch.need_update_devices()) {
        log("push_data");
        parent.trader->schedule_push_data(peer.get_lang());
    }
    if (svc == 0) {
        lock_guard<mutex> lock(mx);
        reset();
        return ok;
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, svc, blob));
}

