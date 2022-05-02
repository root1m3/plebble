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
#include "users_t.h"
#include <us/gov/io/cfg1.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/wallet/local_api.h>
#include "types.h"

#define loglevel "wallet/engine"
#define logclass "users_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::users_t;

c::users_t(daemon_t& daemon): daemon(daemon) {
}

c::~users_t() {
    lock_guard<mutex> lock(mx);
    for (auto i: *this) {
        delete i.second;
    }
}

us::wallet::wallet::local_api* c::get_wallet(const string& subhome) {
    log("get_traders", subhome);
    auto subhomeh = hasher_t::digest(subhome);
    wallet::local_api* o;
    {
        lock_guard<mutex> lock(mx);
        auto i = find(subhomeh);
        if (likely(i != end())) {
            log("found instance of traders", subhomeh);
            return i->second;
        }
        log("creating new instance of wallet subhome", subhome);
        trader::endpoint_t ep(daemon.id.pub.hash(), subhome);
        ep.chan = daemon.channel;
        auto whome = daemon.wallet_home(subhome);
        o = new us::wallet::wallet::local_api(daemon, whome, subhome, subhomeh, move(ep));
        emplace(subhomeh, o);
    }
    if (subhome.empty()) {
        string govhomedir = daemon.home + "../gov";
        if (us::gov::io::cfg1::file_exists(us::gov::io::cfg_id::k_file(govhomedir))) {
            auto k = us::gov::io::cfg1::load_sk(govhomedir);
            if (is_ok(k.first)) {
                hash_t addr = o->add_address(k.second);
                log("added income address", addr);
            }
        }
    }
    return o;
}

void c::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    for (auto i: *this) {
        if (i.first.is_zero()) continue;
        os << "subhome: /" << i.first << '\n';
        i.second->dump("  ", os);
    }
    auto i = find(hash_t(0));
    if (i != end() ) {
        os << "subhome: /" << i->first << '\n';
        i->second->dump("  ", os);
    }
}

void c::sync(ostream& os) {
    log("users_t::sync");
    lock_guard<mutex> lock(mx);
    for (auto i: *this) {
        i.second->sync(os);
    }
}

