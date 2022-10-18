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
#include "bootstrapper_t.h"
#include <mutex>

#include <us/gov/crypto/types.h>
#include <us/gov/socket/datagram.h>

#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>

#include "a_t.h"
#include "b_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "bootstrapper_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::bootstrap::bootstrapper_t;

c::bootstrapper_t(): dialogue_a(*this), dialogue_b(*this), dialogue_c(*this), trade_id(0) {
    log("constructor", this);
}

c::~bootstrapper_t() {
    log("destructor", this, peer);
    if (peer == nullptr) {
        return;
    }
    --peer->sendref;
    peer = nullptr;
}

pair<ko, hash_t> c::start(trader_t& tder) {
    trader = &tder;
    return make_pair(ok, hash_t(0));
}

void c::online(peer_t& cli) {
    log("online", "TRACE 8c");
    peer = &cli;
    ++peer->sendref;
}

void c::offline() {
    log("offline");
    if (peer == nullptr) return; //peer's been disconnected before completing handshake
    --peer->sendref;
    peer = nullptr;
}

namespace {

    using peer_t = us::wallet::engine::peer_t;

    template<typename t>
    pair<ko, string> xtractwloc(us::gov::io::blob_reader_t& reader) {
        t hs;
        auto r = reader.read(hs);
        if (us::is_ko(r)) {
            return make_pair(r, "");
        }
        return make_pair(ok, hs.wloc);
    }

    template<typename D, typename t>
    ko handshake(peer_t& peer, D& d, us::gov::io::blob_reader_t& reader) {
        t hs;
        auto r = reader.read(hs);
        if (us::is_ko(r)) {
            return r;
        }
        return d.handshake(peer, move(hs));
    }

}

pair<ko, string> c::extract_wloc(uint16_t svc, const blob_t& blob) {
    io::blob_reader_t reader(blob);
    switch(svc) {
        case trader_t::svc_handshake_a1: return xtractwloc<a1_t>(reader);
        case trader_t::svc_handshake_c1: return xtractwloc<c1_t>(reader);
    }
    auto r = "KO 92011 Invalid handshake service.";
    log(r);
    return make_pair(r, "");
}

ko c::trading_msg(peer_t& peer, uint16_t svc, blob_t&& blob) {
    log("trading_msg_bootstrapper", svc);
    static_assert((uint16_t)trader_t::svc_begin < (uint16_t)trader_t::svc_handshake_begin);
    static_assert((uint16_t)trader_t::svc_handshake_begin < (uint16_t)trader_t::svc_handshake_end);
    static_assert((uint16_t)trader_t::svc_handshake_end < (uint16_t)trader_t::svc_end);
    io::blob_reader_t reader(blob);
    switch(svc) {
        case trader_t::svc_handshake_a1: return handshake<dialogue_a_t, a1_t>(peer, dialogue_a, reader);
        case trader_t::svc_handshake_a2: return handshake<dialogue_a_t, a2_t>(peer, dialogue_a, reader);
        case trader_t::svc_handshake_a3: return handshake<dialogue_a_t, a3_t>(peer, dialogue_a, reader);

        case trader_t::svc_handshake_b1: return handshake<dialogue_b_t, b1_t>(peer, dialogue_b, reader);
        case trader_t::svc_handshake_b2: return handshake<dialogue_b_t, b2_t>(peer, dialogue_b, reader);
        case trader_t::svc_handshake_b3: return handshake<dialogue_b_t, b3_t>(peer, dialogue_b, reader);

        case trader_t::svc_handshake_c1: return handshake<dialogue_c_t, c1_t>(peer, dialogue_c, reader);
        case trader_t::svc_handshake_c2: return handshake<dialogue_c_t, c2_t>(peer, dialogue_c, reader);
        case trader_t::svc_handshake_c3: return handshake<dialogue_c_t, c3_t>(peer, dialogue_c, reader);
        case trader_t::svc_handshake_c4: return handshake<dialogue_c_t, c4_t>(peer, dialogue_c, reader);
    }
    auto r = "KO 92010 Invalid handshake service.";
    log(r);
    return r;
}


