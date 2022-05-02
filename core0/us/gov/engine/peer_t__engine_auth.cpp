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
#include <us/gov/engine/protocol.h>
#include "daemon_t.h"
#include "db_t.h"
#include "protocol.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "peer_t__engine_auth"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/c++/gov/engine_auth/cllr_rpc-impl>

bool c::process_work__engine_auth(datagram* d) {
    switch(d->service) {
        using namespace protocol;
        #include <us/api/generated/c++/gov/engine_auth/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/c++/gov/engine_auth/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/c++/gov/engine_auth/hdlr_local-impl

ko c::handle_nodes(vector<tuple<hash_t, uint32_t, uint16_t>>& seeds) {
    log("nodes");
    auto& db = engine_daemon().db->auth_app->db;
    db.collect(seeds);
    log("gov::protocol::gov_engine_auth_nodes returning", seeds.size(), "gov endpoints");
    return ok;
}

ko c::handle_lookup_node(hash_t&& addr, lookup_node_out_t& o_out) {
    log("lookup_node");
    /// out:
    ///    uint32_t net_addr;
    ///    uint16_t port;

    auto& db = engine_daemon().db->auth_app->db;
    auto r = db.lookup(addr);
    if (is_ko(r.first)) {
        return r.first;
    }
    o_out.net_addr = r.second.net_address;
    o_out.port = r.second.port;
    return ok;
}

//-/----------------apitool - End of API implementation.

