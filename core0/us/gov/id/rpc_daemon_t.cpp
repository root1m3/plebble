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
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/id"
#define logclass "rpc_daemon_t"
#include "logs.inc"

using namespace us::gov::id;
using c = us::gov::id::rpc_daemon_t;

socket::client* c::create_client() {
    log("create_client");
    return new peer_t(*this);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "id::rpc_daemon_t: caller public key: " << get_keys().pub << " addr " << get_keys().pub.hash() << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

