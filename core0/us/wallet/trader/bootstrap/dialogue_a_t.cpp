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
#include "dialogue_a_t.h"

#include <us/gov/crypto/types.h>
#include <us/gov/socket/datagram.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#include "a_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "dialogue_a_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::bootstrap::dialogue_a_t;

c::dialogue_a_t(bootstrapper_t& parent): parent(parent) {
    log("dialogue_a constructor", &parent);
}

c::~dialogue_a_t() {
    log("dialogue_a destructor");
    delete state;
}

c::state_t* c::release_() {
    log("release_a", &parent);
    auto x = state;
    state = nullptr;
    return x;
}

void c::reset() {
    log("dialogue_a_t::reset", &parent);
    delete state;
    state = nullptr;
}

ko c::initiate(peer_t& peer, const string& wloc) { //exec by initiator
    log("dialogue_a_t::initiate");
    unique_lock<mutex> lock(mx);
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    state = new state_t();
    ++state->seq;
    lock.unlock();
    assert(parent.trader->w != nullptr);
    blob_t blob;
    {
        {
        lock_guard<mutex> lock(parent.trader->mx);
        a1_t o(parent.trader->w->local_endpoint, wloc, parent.trader->parent.published_protocols(false), parent.trader->my_challenge);
        o.write(blob);
        }
    }
    log("sending over a1. sz ", blob.size());
    return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, trader_t::svc_handshake_a1, blob));
}

ko c::handshake(peer_t& peer, a1_t&& o) { /// exec by follower  / does push_data
    log("dialogue_a_t::handshake_1");
    if (!parent.trader->parent.daemon.has_home(o.wloc)) {
        auto r = "KO 80795 Invalid wallet.";
        log(r);
        return r;
    }
    unique_lock<mutex> lock(mx);
    if (state != nullptr) {
        log("restarting");
        reset();
    }
    {
        log("on_a1");
        auto r = parent.on_a(1);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    state = new state_t();
    ++state->seq;
    //peer.tder = parent.trader;
    lock.unlock();
    auto w = parent.trader->parent.daemon.users.get_wallet(o.wloc);
    parent.trader->init(parent.trade_id, o.endpoint, *w);
    parent.trader->online(peer);
    ch_t ch(0);
    {
        log("a1 brought ");
        auto r = parent.trader->deliver(move(o.protocols), move(o.challenge), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    return update_peer2(peer, move(ch));
}

ko c::handshake(peer_t& peer, a2_t&& o) { //exec by initiator, ; ////does push_data
    log("dialogue_a_t::handshake_2");
    unique_lock<mutex> lock(mx);
    if (state == nullptr || state->seq != 1) {
        auto r = KO_43342;
        log(r, state->seq);
        reset();
        return r;
    }
    {
        log("on_a2");
        auto r = parent.on_a(2);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    lock.unlock();
    ch_t ch(0);
    {
        log("a2 brought personality", o.personality_proof.raw().to_string());
        auto r = parent.trader->deliver(move(o.endpoint), move(o.protocols), move(o.challenge), move(o.personality_proof), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    update_peer(peer, move(ch));
    return ok;
}

ko c::handshake(peer_t& peer, a3_t&& o) { //exec by follower, and sometimes by initiator ; //// push_data or not
    log("dialogue_a_t::handshake_3", &parent);
    unique_lock<mutex> lock(mx);
    if (state == nullptr) {
        auto r = KO_43342;
        log(r, state->seq);
        reset();
        return r;
    }
    {
        log("on_a3");
        auto r = parent.on_a(3);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    if (++state->seq > 10) { //loop
        auto r = "KO 68875 In a loop.";
        log(r);
        reset();
        return r;
    }
    lock.unlock();
    ch_t ch(0);
    {
        log("a3 brought personality", o.personality_proof.raw().to_string());
        auto r = parent.trader->deliver(move(o.personality_proof), ch);
        if (is_ko(r)) {
            reset();
            return r;
        }
    }
    update_peer(peer, move(ch));
    return ok;
}

ko c::update_peer(peer_t& peer, ch_t&& ch) {
    log("update_peer", ch.to_string());
    assert(ch.closed());
    if (ch.personality_changed) {
        uint16_t svc;
        log("dialogue_a. update_peer - personality_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        assert(parent.trader->peer_challenge.is_not_zero());
        a3_t x(parent.trader->my_personality.gen_proof(parent.trader->peer_challenge));
        blob_t blob;
        x.write(blob);
        return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, trader_t::svc_handshake_a3, blob));
    }
    if (ch.need_update_devices()) {
        log("push_data");
        parent.trader->schedule_push_data(peer.get_lang());
    }
    return ok;
}

ko c::update_peer2(peer_t& peer, ch_t&& ch) {
    log("update_peer", ch.to_string());
    assert(ch.closed());
    ostringstream msg;
    if (true || ch.personality_changed) {
        log("dialogue_a. update_peer2 - personality_changed", "using challenge", parent.trader->peer_challenge, "@", &parent);
        lock_guard<mutex> lock(parent.trader->mx);
        auto proof = parent.trader->my_personality.gen_proof(parent.trader->peer_challenge);
        assert(parent.trader->w != nullptr);
        a2_t x(parent.trader->w->local_endpoint, parent.trader->parent.published_protocols(false), move(proof), parent.trader->my_challenge);
        blob_t blob;
        x.write(blob);
        return peer.call_trading_msg(peer_t::trading_msg_in_t(parent.trader->id, trader_t::svc_handshake_a2, blob));
    }
    if (ch.need_update_devices()) {
        log("push_data");
        parent.trader->schedule_push_data(peer.get_lang());
    }
    return ok;
}

