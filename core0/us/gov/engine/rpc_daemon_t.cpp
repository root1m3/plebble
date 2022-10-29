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
#include "rpc_daemon_t.h"

#include <us/gov/config.h>

#include "rpc_peer_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "rpc_daemon_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::rpc_daemon_t;

c::rpc_daemon_t(channel_t channel, dispatcher_t* dispatcher): b(channel, dispatcher) {
    api_v = CFG_API_V__GOV;
    assert(api_v != 0);
    log("set api_v", +api_v);
}

c::~rpc_daemon_t() {
}

socket::client* c::create_client() {
    log("create_client");
    return new rpc_peer_t(*this);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "engine::rpc_daemon_t\n";
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

