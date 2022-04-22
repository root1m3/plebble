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
#include "doctype_processors_t.h"
#include <us/gov/crypto/base58.h>

#define loglevel "wallet/trader/workflow"
#define logclass "doctype_processors"
#include <us/gov/logs.inc>

using c = us::wallet::trader::workflow::doctype_processors_t;
using p = us::wallet::trader::workflow::doctype_processor_t;
using us::ko;

c::doctype_processors_t() {
}

void c::add(magic_t m, const string& name, const string& ep) {
    add(m, doctype_processor_t(name, ep));
}

void c::add(magic_t m, doctype_processor_t&&p) {
    assert(!p.first.empty());
    assert(!p.second.empty());
    auto i = find(m);
    if (i == end()) {
        i = emplace(m, vector<doctype_processor_t>()).first;
    }
    i->second.emplace_back(move(p));
}

using namespace us::gov::crypto;

void p::to_stream(ostream& os) const {
    os << b58::encode(first) << ' ' << b58::encode(second) << ' ';
}

void c::to_stream(ostream& os) const {
    log("saving verion", version);
    os << +version << ' ';
    os << size() << ' ';
    for (auto&i :*this) {
        os << i.first << ' ';
        os << i.second.size() << ' ';
        for (auto& j :i.second) {
            j.to_stream(os);
        }
    }
}

ko p::from_stream(istream& is) {
    string name;
    string ep;
    is >> name;
    is >> ep;
    if (is.fail()) {
        auto r = "KO 88097";
        log(r);
        return r;
    }
    first = b58::decode_string(name);
    second = b58::decode_string(ep);
    if (first.empty() || second.empty()) {
        auto r = "KO 88098";
        log(r);
        return r;
    }
    return ok;
}

ko c::from_stream(istream& is) {
    uint8_t version;
    is >> version;
    log("loading verion", version);
    size_t n;
    is >> n;
    if (is.fail()) {
        auto r = "KO 88096";
        log(r);
        return r;
    }
    for (size_t i=0; i<n; ++i) {
        magic_t doctype;
        int m;
        is >> doctype;
        is >> m;
        if (is.fail()) {
            auto r = "KO 88098";
            log(r);
            return r;
        }
        for (size_t j = 0; j < m; ++j) {
            doctype_processor_t p;
            auto r = p.from_stream(is);
            if (is_ko(r)) {
                return r;
            }
            add(doctype, move(p));
        }
    }
    log("loaded ok");
    return ok;
}

ko c::load(const string& filename) {
    log("loading", filename);
    ifstream is(filename);
    return from_stream(is);
}

ko c::save(const string& filename) {
    log("saving", filename);
    ofstream os(filename);
    to_stream(os);
    return ok;
}

