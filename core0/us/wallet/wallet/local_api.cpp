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
#include "local_api.h"
#include <sstream>

#include <us/gov/io/cfg1.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash/addresses_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/wallet/tx_make_p2pkh_input.h>
#include <us/wallet/trader/bookmarks_t.h>

#include "types.h"

#define loglevel "wallet/wallet"
#define logclass "local_api"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::local_api;
using peer_t = us::wallet::engine::peer_t;

c::local_api(engine::daemon_t& daemon, const string& home, const string& subhome, const hash_t& subhomeh, trader::endpoint_t&& ep):
        daemon(daemon),
        home(home),
        subhome(subhome),
        subhomeh(subhomeh),
        device_filter([&](const socket::client& peer) -> bool {
                assert(static_cast<const peer_t&>(peer).wallet != nullptr);
                log("filter: peer subhome", static_cast<const peer_t&>(peer).wallet->subhome, "subhomeh", static_cast<const peer_t&>(peer).wallet->subhomeh, "==", this->subhomeh);
                return static_cast<const peer_t&>(peer).wallet->subhomeh == this->subhomeh;
            }),
        w(home + "/keys"),
        local_endpoint(ep) {
}

c::~local_api() {
}

ko c::refresh_data() {
    log("refresh_data");
    if (unlikely(empty())) {
        auto r = "KO 20183 Wallet contains 0 addresses.";
        log(r);
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = nullptr;
        return r;
    }
    us::gov::cash::addresses_t addresses;
    addresses.reserve(size());
    for (auto& i: *this) {
        addresses.emplace_back(i.first);
    }

    accounts_t* o = new accounts_t();
    auto r = daemon.gov_rpc_daemon.get_peer().call_fetch_accounts(addresses, *o);
    if (is_ko(r)) {
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = nullptr;
        return r;
    }
    {
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = o;
    }
    return ok;
}

void c::dump(const string& pfx, ostream& os) const {
}

