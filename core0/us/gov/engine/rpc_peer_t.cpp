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
#include "rpc_peer_t.h"
#include "types.h"
#include "svcfish_t.h"
#include "rpc_daemon_t.h"
#include "net_daemon_t.h"
#include "daemon_t.h"

#define loglevel "gov/engine"
#define logclass "rpc_peer_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::rpc_peer_t;

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "engine: \n";
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

svc_t c::translate_svc(svc_t svc0, bool inbound) const {
    svc_t svc;
    if (inbound) {
        svc = daemon_t::svcfish.from_prev(svc);
        log("Using API versioning translator. oldsvc ", svc0, "-> newsvc", svc);
    }
    else {
        svc = daemon_t::svcfish.to_prev(svc);
        log("Using API versioning translator. newsvc ", svc0, "-> oldsvc", svc);
    }
    return svc;    
}

bool c::process_work(datagram* d) {
    log("process_work", d->service);
    using namespace protocol;
    /// Assuming the following svc ordering:
    /// master definition: us/api/model.cpp
    static_assert(socket_end <= id_end);
    static_assert(id_end <= auth_end);
    static_assert(auth_end <= peer_end);
    static_assert(peer_end <= relay_end);
    static_assert(relay_end <= dfs_end);
    static_assert(dfs_end <= engine_end);
    static_assert(engine_end <= cash_end);
    static_assert(cash_end <= engine_auth_end);
    static_assert(engine_auth_end <= traders_end);
    static_assert(traders_end <= sys_end);
    using namespace us::gov::protocol;
    if (d->service < net_daemon_end) {
        log("processing net_daemon datagram", d->service);
        return b::process_work(d);
    }
    return false;
}

#include <us/api/generated/gov/c++/engine/cllr_rpc-impl>
#include <us/api/generated/gov/c++/engine_auth/cllr_rpc-impl>
#include <us/api/generated/gov/c++/cash/cllr_rpc-impl>
#include <us/api/generated/gov/c++/traders/cllr_rpc-impl>
#include <us/api/generated/gov/c++/sys/cllr_rpc-impl>

