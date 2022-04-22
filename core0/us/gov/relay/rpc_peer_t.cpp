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
#include "rpc_peer_t.h"
#include <us/gov/socket/rpc_daemon_t.h>

#define loglevel "gov/relay"
#define logclass "rpc_peer_t"
#include "logs.inc"

using namespace us::gov::relay;
using c = us::gov::relay::rpc_peer_t;

bool c::process_work(datagram* d) {
    using namespace us::gov::protocol;
    assert(d != nullptr);
    assert(d->service < protocol::relay_end);
    if (d->service == relay_push) {
        if (static_cast<socket::rpc_daemon_t&>(daemon).dispatcher == nullptr) {
            log("discarding push datagram. dispatcher not available.");
            delete d;
            return true;
        }
        log("push datagram handed over to dispatcher");
        if (!static_cast<socket::rpc_daemon_t&>(daemon).dispatcher->dispatch(d)) {
            log("Not consumed");
            delete d;
        }
        return true;
    }
    return b::process_work(d);
}

#ifdef has_us_gov_relay_api
#include <us/api/generated/c++/gov/relay/cllr_rpc-impl>
#endif

