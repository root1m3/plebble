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
#include "nodes_t.h"
#include "protocol.h"
#include <chrono>
#include <random>
#include <us/gov/io/cfg.h>
#include <us/gov/socket/client.h>

#define loglevel "gov/peer"
#define logclass "nodes_t"
#include "logs.inc"

using c = us::gov::peer::nodes_t;
using namespace us::gov;

const char* c::KO_82978 = "KO 82978 Entry not found.";
const char* c::KO_92276 = "KO 92276 Empty set.";

c::nodes_t(const nodes_t& other): b(other) {
}

void c::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << i.first << ' '; i.second.dump(os);
    }
}

void c::dump_as_seeds(ostream& os) const {
    for (auto& i: *this) {
        i.second.dump_as_seeds(os);
    }
}

void c::collect(vector<tuple<hash_t, host_t, port_t>>& v) const {
    v.clear();
    v.reserve(size());
    for (auto& i: *this) {
        v.emplace_back(make_tuple(i.first, i.second.net_address, i.second.port));
    }
}

void c::collect(vector<hash_t>& v) const {
    v.clear();
    v.reserve(size());
    for (auto& i: *this) {
        v.emplace_back(i.first);
    }
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        i.second.hash_data_to_sign(h);
    }
}

void c::hash_data(hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        i.second.hash_data(h);
    }
}

us::ko c::check(const hash_t& id, const account_t& ac, channel_t channel) {
    if (unlikely(id.is_zero())) {
        auto r = "KO 22153 Invalid Address.";
        log(r);
        return r;
    }
    return ac.check(channel);
}

pair<ko, peer::account_t> c::lookup(const hash_t& p) const {
    auto i = find(p);
    if (i == end()) {
        log(KO_82978, p);
        return make_pair(KO_82978, account_t());
    }
    log("found: ", socket::client::endpoint(i->second.net_address, i->second.port));
    return make_pair(ok, i->second);
}

pair<ko, tuple<hash_t, host_t, port_t>> c::random_entry() const {
    static mt19937_64 rng{random_device{}()};
    log("random_entry", size());
    if (empty()) {
        log(KO_92276);
        return make_pair(KO_92276, make_tuple(hash_t(0), 0, 0));
    }
    uniform_int_distribution<> d(0, size() - 1);
    auto i = next(begin(), d(rng));
    log("random entry index", distance(begin(), i), "sz", size());
    return make_pair(ok, make_tuple(i->first, i->second.net_address, i->second.port));
}

