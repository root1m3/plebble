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
#include <functional>
#include <random>

#include <us/gov/config.h>
#include <us/gov/auth/peer_t.h>
#include <us/gov/engine/daemon_t.h>

#include "wallet_address.h"
#include "types.h"
#include "db_t.h"

#define loglevel "gov/traders"
#define logclass "db_t"
#include <us/gov/logs.inc>

using namespace us::gov::traders;
using c = us::gov::traders::db_t;

c::db_t() {
}

void c::clear() {
    lock_guard<mutex> lock(mx_wallets);
    wallets.clear();
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "wallets:\n";
    lock_guard<mutex> lock(mx_wallets);
    wallets.dump(prefix + "    ", os);
    os << prefix << wallets.size() << " entries.\n";
}

void c::collect(vector<tuple<hash_t, host_t, port_t>>& v) const {
    log("collect_endpoints");
    lock_guard<mutex> lock(mx_wallets);
    wallets.collect(v);
}

void c::collect(vector<hash_t>& v) const {
    log("collect_endpoints2");
    lock_guard<mutex> lock(mx_wallets);
    wallets.collect(v);
}

pair<ko, c::account_t> c::lookup(const hash_t& h) const {
    lock_guard<mutex> lock(mx_wallets);
    return wallets.lookup(h);
}

pair<ko, tuple<hash_t, host_t, port_t>> c::random_wallet() const {
    lock_guard<mutex> lock(mx_wallets);
    return wallets.random_entry();
}

void c::hash(hasher_t& h) const {
    lock_guard<mutex> lock(mx_wallets);
    for (auto& i: wallets) {
        h << i.first << i.second.net_address << i.second.port << i.second.seen;
    }
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(wallets);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(wallets);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(wallets);
        if (is_ko(r)) return r;
    }
    return ok;
}

