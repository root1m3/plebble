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
#include "dialogue_b_t.h"

#include <us/gov/crypto/types.h>
#include <us/gov/socket/datagram.h>

#include <us/wallet/protocol.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>

#include "b_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "dialogue_b_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::bootstrap::dialogue_b_t;

c::dialogue_b_t(bootstrapper_t& parent): parent(parent) {
    log("dialogue_b constructor", &parent);
}

c::~dialogue_b_t() {
    log("dialogue_b destructor");
    delete state;
}

c::state_t* c::release_() {
    log("release_b", &parent);
    auto x = state;
    state = nullptr;
    return x;
}

void c::reset() {
    log("dialogue_b_t::reset", &parent);
    delete state;
    state = nullptr;
}

ko c::initiate(peer_t& peer, protocol_selection_t&& protocol_selection, ch_t& ch) {
    log("dialogue_b_t::initiate", &parent);
    unique_lock<mutex> lock(mx);
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    state = new state_t();
    auto p = parent.trader->parent.create_opposite_protocol(protocol_selection_t(protocol_selection));
    if (p.first != ok) {
        log(p.first);
        assert(p.second == nullptr);
        reset();
        return move(p.first);
    }
    auto r = parent.trader->set_protocol(p.second, ch);
    if (is_ko(r)) {
        reset();
        return r;
    }
    lock.unlock();
    blob_t blob;
    {
        lock_guard<mutex> lock(parent.trader->mx);
        b1_t x(protocol_selection, parent.trader->shared_params());
        x.write(blob);
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, trader_t::svc_handshake_b1, blob));
}

ko c::handshake(peer_t& peer, b1_t&& o) {
    log("dialogue_b_t::handshake_1", "seq=0");
    unique_lock<mutex> lock(mx);
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    state = new state_t();
    ++state->seq;
    lock.unlock();
    ch_t ch(0);
    {
        auto r = parent.trader->deliver(move(o.protocol_selection), move(o.params), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    return update_peer(peer, move(ch), true);
}

ko c::handshake(peer_t& peer, b2_t&& o) {
    unique_lock<mutex> lock(mx);
    log("dialogue_b_t::handshake_2", (state != nullptr ? state->seq : -1) );
    if (state == nullptr) {
        auto r = KO_43343;
        log(r);
        reset();
        return r;
    }
    if (++state->seq > 10) { //loop
        auto r = "KO 68885 In a loop.";
        log(r);
        reset();
        return r;
    }
    lock.unlock();
    ch_t ch(0);
    {
        log("b2 brought personality", o.personality_proof.raw().to_string());
        auto r = parent.trader->deliver(move(o.personality_proof), move(o.params), ch);
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

ko c::handshake(peer_t& peer, b3_t&& o) {
    unique_lock<mutex> lock(mx);
    log("dialogue_b_t::handshake_3", (state != nullptr ? state->seq : -1) );
    if (state == nullptr) {
        auto r = KO_43343;
        log(r);
        reset();
        return r;
    }
    if (++state->seq > 10) { //loop
        auto r = "KO 68885 In a loop.";
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

ko c::update_peer(peer_t& peer, ch_t&& ch, bool b) {
    log("update_peer", ch.to_string());
    assert(ch.closed());
    //ostringstream msg;
    svc_t svc;
    blob_t blob;
    if (b || (ch.personality_changed && ch.shared_params_changed)) {
        log("dialogue_b. update_peer - personality_changed", "shared_params_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        assert(parent.trader->peer_challenge.is_not_zero());
        b2_t x(parent.trader->my_personality.gen_proof(parent.trader->peer_challenge), parent.trader->shared_params());
        x.write(blob);
        svc = trader_t::svc_handshake_b2;
    }
    else if (ch.personality_changed) {
        log("dialogue_b. update_peer - personality_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        assert(parent.trader->peer_challenge.is_not_zero());
        b2_t x(parent.trader->my_personality.gen_proof(parent.trader->peer_challenge));
        x.write(blob);
        svc = trader_t::svc_handshake_b2;
    }
    else if (ch.shared_params_changed) {
        log("dialogue_b. update_peer - shared_params_changed");
        lock_guard<mutex> lock(parent.trader->mx);
        b3_t x(parent.trader->shared_params());
        x.write(blob);
        svc = trader_t::svc_handshake_b3;
    }
    else {
        log("dialogue_b. update_peer - no changes.");
        svc = 0;
    }
    if (ch.need_update_devices()) {
        log("push_data");
        parent.trader->schedule_push_data(peer.get_lang());
    }
    if (svc == 0) {
        return ok;
    }
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, svc, blob));
}

