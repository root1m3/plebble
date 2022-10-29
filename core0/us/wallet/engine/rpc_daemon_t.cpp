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
#include <sstream>
#include <chrono>
#include <thread>

#include <us/gov/config.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/id/peer_t.h>

#include "rpc_peer_t.h"

#define loglevel "wallet/engine"
#define logclass "rpc_daemon_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::rpc_daemon_t;

c::rpc_daemon_t(channel_t channel, dispatcher_t* dispatcher): b(channel, dispatcher) {
    stop_on_disconnection = false;
    connect_for_recv = false;

    api_v = CFG_API_V__WALLET;
    assert(api_v != 0);
    log("set api_v", +api_v);
}

c::~rpc_daemon_t() {
}

