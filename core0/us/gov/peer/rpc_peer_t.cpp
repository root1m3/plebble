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
#include "rpc_daemon_t.h"
#include "protocol.h"

#define loglevel "gov/peer"
#define logclass "rpc_peer_t"
#include "logs.inc"

using namespace us::gov::peer;
using c = us::gov::peer::rpc_peer_t;

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "  rpc_peer\n";
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

#ifdef has_us_gov_peer_api
    #include <us/api/generated/c++/gov/peer/cllr_rpc-impl>
#endif

