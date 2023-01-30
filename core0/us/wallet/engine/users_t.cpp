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
#include <us/wallet/trader/businesses_t.h>

#include "types.h"

#define loglevel "wallet/engine"
#define logclass "users_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::users_t;

c::users_t(daemon_t& daemon): daemon(daemon) {
}

c::~users_t() {
    {
        lock_guard<mutex> lock(mx);
        for (auto i: *this) {
            auto r = "KO 78869 wallet wasn't released before. LEAK";
            log(r, i.second->subhome);
            //delete i.second;
        }
    }
    release_wallet(root_wallet);
}

void c::init() {
    string govhomedir = daemon.home + "/../gov"; //is there a gov process running along with this wallet process? manage its key
    log("root wallet instantiation", "make sure it contains gov income address. looking at dir", govhomedir);
    root_wallet = get_wallet("");
    assert(root_wallet != nullptr);
    if (us::gov::io::cfg1::file_exists(us::gov::io::cfg_id::k_file(govhomedir))) {
        log("found gov/k file");
        auto k = us::gov::io::cfg1::load_sk(govhomedir);
        if (is_ko(k.first)) {
            log("gov key is not available at", govhomedir);
        }
        hash_t addr = root_wallet->add_address(k.second);
        log("added income address", addr);
    }
}

us::wallet::wallet::local_api* c::get_wallet(const string& subhome) {
    log("get_wallet", subhome);
    auto subhomeh = hasher_t::digest(subhome);
    wallet::local_api* o;
    lock_guard<mutex> lock(mx);
    auto i = find(subhomeh);
    if (likely(i != end())) {
        log("wallet local_api instance is", i->second, "subhome=", subhome);
        return i->second;
    }
    log("creating new instance of wallet subhome", subhome);
    trader::endpoint_t ep(daemon.id.pub.hash(), subhome);
    ep.chan = daemon.channel;
    auto whome = daemon.wallet_home(subhome);
    assert(ep.pkh.is_not_zero());
    log("wallet instantiation", ep.to_string());
    o = new us::wallet::wallet::local_api(daemon, whome, subhome, subhomeh, move(ep));
    ++o->refcount;
    emplace(subhomeh, o);
    o->start();
    log("wallet local_api instance is", o, "subhome=", subhome);
    return o;
}

void c::release_wallet(wallet::local_api* w) {
    if (unlikely(w == nullptr)) {
        auto r = "KO 89798 release of a null wallet local_api instance.";
        log(r);
        return;
    }
    lock_guard<mutex> lock(mx);
    --w->refcount;
    if (w->refcount > 0) return;
    auto i = find(w->subhomeh);
    if (unlikely(i == end())) {
        auto r = "KO 89799 wallet local_api instance not found. Couldn't release instance.";
        log(r);
        return;
    }
    assert(w == i->second);
    erase(i);
    w->stop();
    w->join();
    delete w;
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

void c::on_tx_tracking_status(const track_status_t& status) {
    log("on_tx_tracking_status", size());
    lock_guard<mutex> lock(mx);
    for (auto i: *this) {
        i.second->on_tx_tracking_status(status);
    }
}

