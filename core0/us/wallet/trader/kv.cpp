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
#include "kv.h"
#include <set>
#include <fstream>
#include <b64/decode.h>
#include <us/gov/crypto/base64.h>

#define loglevel "wallet/trader"
#define logclass "kv"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::kv;

bool c::operator != (const c& other) const {
    if (size() != other.size()) return true;
    const_iterator i1 = begin(), i2 = other.begin();
    while(i1 != end()) {
        if (i1->first != i2->first) return true;
        if (i1->second != i2->second) return true;
        ++i1; ++i2;
    }
    return false;
}

bool c::override_with(const c& other) { //can be done more efficiently
    bool r = false;
    for (auto& i: other) {
        auto x = emplace(i);
        r |= x.second;
        if (x.first->second != i.second) {
            r = true;
            x.first->second = i.second;
        }
    }
    return r;
}

void c::hash(sigmsg_hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

void c::hash(hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

string c::to_b64() const {
    ostringstream os;
    for (auto& i: *this) {
        os << i.first << ' ' << i.second << '\n';
    }
    return us::gov::crypto::b64::encode_string(os.str());
}

namespace {
    void tabdump(const string& pfx, char* b, size_t sz, ostream& os) {
        size_t p = 0;
        while (p < sz) {
            size_t cr = p;
            while(cr < sz && b[cr] != '\n') ++cr;
            b[cr] = '\0';
            os << pfx << &b[p] << '\n';
            p = cr + 1;
        }
    }
}

bool c::dump_b64(const string& prefix, const string& keyname, const string& value, ostream& os) {
    if (keyname.size() < 4) return false;
    if (keyname.substr(keyname.size() - 4) != "_b64") return false;
    if (value.empty()) return true;
    base64::decoder d;
    char b[value.size()];
    int sz = d.decode(value.data(),value.size(),b);
    if (likely(sz >= 0)) {
        os << prefix << keyname.substr(0, keyname.size() - 4) << ":\n";
        tabdump(prefix+"  ", b, sz, os);
    }
    else {
        os << "KO b64 key " << keyname << '\n';
    }
    return true;
}

string c::hide_skvalue(const string& keyname, const string& value) {
    if (keyname.size() < 3) return value;
    if (keyname.substr(keyname.size() - 3) == "_sk") { //suffix in key name indicates value is secret
        if (value.empty()) return value;
        return "********";
    }
    return value;
}

void c::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        if (dump_b64(prefix, i.first, i.second, os)) {
            continue;
        }
        os << prefix << i.first << ' ' << hide_skvalue(i.first, i.second) << '\n';
    }
}

void c::write_pretty(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        if (dump_b64(prefix, i.first, i.second, os)) {
            continue;
        }
        os << prefix << i.first << ": " << hide_skvalue(i.first, i.second) << '\n';
    }
}

int c::get(const string& key, int def) const {
    auto i = find(key);
    if (i == end()) return def;
    istringstream is(i->second);
    int r;
    is >> r;
    if (is.fail()) return def;
    return r;
}

string c::get(const string& key, const string& def) const {
    auto i = find(key);
    if (i == end()) return def;
    return i->second;
}

hash_t c::get(const string& key, const hash_t& def) const {
    auto i = find(key);
    if (i == end()) return def;
    hash_t h;
    istringstream is(i->second);
    is >> h;
    if (is.fail()) return def;
    return move(h);
}

cash_t c::get(const string& key, const cash_t& def) const {
    auto i = find(key);
    if (i == end()) return def;
    cash_t h;
    istringstream is(i->second);
    is >> h;
    if (is.fail()) return def;
    return move(h);
}

uint64_t c::get(const string& key, const uint64_t& def) const {
    assert(!key.empty());
    auto i = find(key);
    if (i == end()) return def;
    uint64_t h;
    istringstream is(i->second);
    is >> h;
    if (is.fail()) return def;
    return move(h);
}

bool c::sets(const string& key, const string& value) {
    assert(!key.empty());
    log("set kv", key, hide_skvalue(key, value));
    auto& i = (*this)[key];
    if (i == value) return false;
    log("key", key, "; old_value '", (*this)[key], "'; new_value '", value, "'");
    (*this)[key] = value;
    return true;
}

bool c::underrides(const string& key, const string& value) {
    assert(!key.empty());
    log("underride kv", key, hide_skvalue(key, value));
    auto i = find(key);
    if (i == end()) {
        emplace(key, value);
        return true;
    }
    return false;
}

bool c::unset(const string& key) {
    log("unset", key);
    auto i = find(key);
    if (i == end()) {
        return false;
    }
    erase(i);
    return true;
}

bool c::loadX(istream& is) {
    log("loading params from input stream");
    bool ch = false;
    while(is.good()) {
        string line;
        getline(is, line);
        us::gov::io::cfg0::trim(line);
        if (line.empty()) continue;
        istringstream isl(line);
        string key;
        isl >> key;
        string value;
        getline(isl, value);
        us::gov::io::cfg0::trim(value);
        auto x = emplace(key, value);
        ch |= x.second;
        if (x.first->second != value) {
            ch = true;
            x.first->second = value;
        }
    }
    log("params changed?", (ch ? "YES" : "no"));
    return ch;
}

bool c::loadX(const string& file) {
    log("loading params from", file);
    ifstream is(file);
    return loadX(is);
}

bool c::load_b64X(const string& b64) {
    log("loading params from b64");
    string content = us::gov::crypto::b64::decode_string(b64);
    istringstream is(content);
    return loadX(is);
}

void c::saveX(const string& file) const {
    ofstream os(file);
    for (auto& i: *this) {
        os << i.first << ' ' << i.second << '\n';
    }
}

ko c::from_streamX(istream& is) {
    log("from_stream");
    clear();
    size_t sz;
    is >> sz;
    if (is.fail()) {
        auto r = "KO 55948";
        log(r);
        return r;
    }
    for (auto i = 0; i < sz; ++i) {
        string key;
        is >> key;
        string valueb58;
        is >> valueb58;
        if (valueb58 == "-") {
            emplace(key, "");
        }
        else {
            emplace(key, us::gov::crypto::b58::decode_string(valueb58));
        }
        if (is.fail()) {
            auto r = "KO 55949";
            log(r);
            return r;
        }
    }
    return ok;
}

void c::to_streamX(ostream& os) const {
    os << size() << ' ';
    for (auto& i: *this) {
        assert(!i.first.empty());
        os << i.first << ' ';
        if (i.second.empty()) {
            os << "- ";
        }
        else {
            os << us::gov::crypto::b58::encode(i.second) << ' ';
        }
    }
}

