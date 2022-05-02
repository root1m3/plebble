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
#include "initiator_t.h"
#include "../trader_t.h"
#include "../traders_t.h"
#include <us/wallet/wallet/local_api.h>

#define loglevel "wallet/trader"
#define logclass "initiator_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet;
using namespace us::wallet::trader::bootstrap;
using c = us::wallet::trader::bootstrap::initiator_t;
using us::ko;

c::initiator_t(qr_t&& remote_qr, wallet::local_api& w): remote_qr(move(remote_qr)), w(w) {
    log("constructor", "TRACE 8c", "qr:", this, remote_qr.to_string());
}

c::~initiator_t() {
    log("destructor");
}

c::hash_t c::make_new_id() const { //TODO add some entropy. tid can be deduced with three inputs who-who-when
    us::gov::crypto::ripemd160 h;
    w.local_endpoint.hash(h);
    remote_qr.endpoint.hash(h);
    h.write(trader->ts_creation);
    hash_t tid;
    h.finalize(tid); //unique hash for trade-id (tid)
    log("computed new id:", tid);
    return tid;
}

pair<ko, c::hash_t> c::start(trader_t& tder) {
    auto r = b::start(tder);
    log("start", "TRACE 8c");
    if (is_ko(r.first)) {
        return r;
    }
    log("start", "protocol selection", this, remote_qr.protocol_selection.first, remote_qr.protocol_selection.second);
    trade_id = make_new_id();
    log("constructor initiator", trade_id);
    trader->init(trade_id, remote_qr.endpoint, w);
    trader->set_state(conman::state_req_online);
    return make_pair(ok, trade_id);
}

void c::online(peer_t& peer) {
    b::online(peer);
    log("TRACE 8c");
    log("online", trader->has_protocol(), "protocol selection", this, remote_qr.protocol_selection.first, remote_qr.protocol_selection.second);
    if (remote_qr.protocol_selection.first.empty() || remote_qr.protocol_selection.second.empty()) {
        log("spawning dialogue_a");
        dialogue_a.initiate(peer, remote_qr.endpoint.wloc);
    }
    else {
        log("spawning dialogue_c");
        dialogue_c.initiate(peer, remote_qr.endpoint.wloc, remote_qr.protocol_selection);
    }
}

ko c::on_a(int n) {
    log("on_a", n);
    if (n == 1) {
        log(dialogue_a_t::KO_43342);
        return dialogue_a_t::KO_43342;
    }
    return ok;
}

ko c::on_c(int n) {
    if (n == 1) {
        log(dialogue_c_t::KO_43344);
        return dialogue_c_t::KO_43344;
    }
    assert(n == 2 || n == 3 || n == 4);
    return ok;
}

