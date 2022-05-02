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
#include "follower_t.h"
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>

#define loglevel "wallet/trader/bootstrap"
#define logclass "follower_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet::trader::bootstrap;
using c = us::wallet::trader::bootstrap::follower_t;
using us::ko;

c::follower_t(const hash_t& trade_id, peer_t& peer): peer(peer) {
    b::trade_id = trade_id;
    log("constructor");
}

pair<ko, c::hash_t> c::start(trader_t& tder) {
    log("start");
    trader = &tder;
    return make_pair(ok, trade_id);
}

void c::offline() {
    b::offline();
    trader->remote_ip.second = 0; //port is not meaningful as a follower
}

ko c::on_a(int n) {
    log("on_a", n);
    if (n == 2) {
        log(dialogue_a_t::KO_43342);
        return dialogue_a_t::KO_43342;
    }
    return ok;
}

ko c::on_c(int n) {
    if (n == 2) {
        log(dialogue_c_t::KO_43344);
        return dialogue_c_t::KO_43344;
    }
    assert(n == 1 || n == 3 || n ==4);
    return ok;
}

