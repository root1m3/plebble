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
#include "peer_t.h"
#include <us/gov/engine/protocol.h>
#include <us/gov/traders/app.h>
#include "daemon_t.h"
#include "db_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "peer_t__traders"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/c++/gov/traders/cllr_rpc-impl>

bool c::process_work__traders(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/c++/gov/traders/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/c++/gov/traders/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/c++/gov/traders/hdlr_local-impl

ko c::handle_wallets(vector<hash_t>& wallets) {
    log("wallets");
    auto& db = engine_daemon().db->traders_app->db;
    db.collect(wallets);
    return ok;
}

ko c::handle_lookup_wallet(hash_t&& pkh, lookup_wallet_out_t& o_out) {
    log("lookup_wallet");
    /// out:
    ///    uint32_t net_addr;
    ///    uint16_t port;
    log("handle_lookup_wallet", pkh);
    auto& db = engine_daemon().db->traders_app->db;

    //pair<ko, hostport_t> r;
    auto r = db.lookup(pkh);
    if (is_ko(r.first)) {
        log("wallet entry not found for", pkh);
        return r.first;
    }
    o_out.net_addr = r.second.net_address;
    o_out.port = r.second.port;
    log("wallet entry found for", pkh, socket::client::endpoint(o_out.net_addr, o_out.port));
    return ok;
}

ko c::handle_random_wallet(random_wallet_out_t& o_out) {
    log("random_wallet");
    /// out:
    ///    hash_t addr;
    ///    uint32_t net_addr;
    ///    uint16_t port;

    auto& db = engine_daemon().db->traders_app->db;
    auto r = db.random_wallet();
    if (is_ko(r.first)) {
        return r.first;
    }
    o_out.addr = get<0>(r.second);
    o_out.net_addr = get<1>(r.second);
    o_out.port = get<2>(r.second);
    return ok;
}

//-/----------------apitool - End of API implementation.

