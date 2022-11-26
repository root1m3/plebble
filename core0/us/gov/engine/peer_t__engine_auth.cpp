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
//#include <us/gov/io/blob_reader_t.h>
//#include <us/gov/io/blob_writer_t.h>

#include "api.h"
#include "daemon_t.h"
#include "db_t.h"
#include "protocol.h"
#include "types.h"
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/engine"
#define logclass "peer_t__engine_auth"
#include "logs.inc"
#include <us/gov/socket/dto.inc>
/*
namespace {
    using t = us::gov::io::seriable_vector<us::gov::crypto::ripemd160::value_type>;
    template<> datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq, const t& o) {
        static_assert(std::is_convertible<t*, writable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        static_assert(std::is_convertible<t*, seriable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        return o.get_datagram(channel, svc, seq);
    }

    template<> ko read_datagram(const datagram& d, t& o) {
        return o.read(d);
    }
}

namespace {
    using t2 = us::gov::cash::accounts_t;
    template<> datagram* write_datagram(channel_t channel, svc_t svc, seq_t seq, const t2& o) {
        static_assert(std::is_convertible<t2*, writable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        static_assert(std::is_convertible<t2*, seriable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        return o.get_datagram(channel, svc, seq);
    }

    template<> ko read_datagram(const datagram& d, t2& o) {
        static_assert(std::is_convertible<t2*, writable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        static_assert(std::is_convertible<t2*, seriable*>::value, "KO 77897 template specialization for writable should have been instantiated instead.");
        return o.read(d);
    }
}
*/
using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/gov/c++/engine_auth/cllr_rpc-impl>

bool c::process_work__engine_auth(datagram* d) {
    switch(d->service) {
        using namespace protocol;
        #include <us/api/generated/gov/c++/engine_auth/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/gov/c++/engine_auth/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/engine_auth/hdlr_local-impl

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

