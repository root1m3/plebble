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
#include <sstream>
#include "types.h"

#include <us/gov/socket/datagram.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/trader/qr_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#define loglevel "wallet/engine"
#define logclass "peer_t__pairing"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::peer_t;

const char* c::KO_10428 = "KO 10428 Traders unavailale.";

#include <us/api/generated/wallet/c++/r2r/cllr_rpc-impl>

bool c::process_work__r2r(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/wallet/c++/r2r/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/wallet/c++/r2r/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/wallet/c++/r2r/hdlr_local-impl

ko c::handle_trading_msg(seq_t seq, trading_msg_in_dst_t&& o_in) {
    log("trading_msg", seq);
    /// in:
    ///     hash_t tid;
    ///     uint16_t code;
    ///     vector<uint8_t> payload;

    log("trading message arrived for trade", o_in.tid, "code", o_in.code);
    return static_cast<daemon_t&>(daemon).traders.trading_msg(*this, o_in.code, o_in.tid, move(o_in.payload));
/*
    if (is_ko(r)) {
        log("unable to process trading msg, disconnecting peer.", r);
        disconnect(0, r);
        return r;
    }
    return ok;
*/
}

//-/----------------apitool - End of API implementation.

