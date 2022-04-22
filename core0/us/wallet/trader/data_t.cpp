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
#include "data_t.h"

#include <us/gov/io/cfg0.h>
#include <us/gov/crypto/base58.h>

#include "trader_t.h"

#define loglevel "wallet/trader"
#define logclass "data_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::data_t;

c::data_t() {
}

c::~data_t() {
    log("destructor", size(), this);
}

c::data_t(const data_t& other): b(other) {
}

c::data_t(data_t&& other): b(move(other)) {
}


template<> void c::add(const string& key, const string& value) {
    emplace(key, value);
}

bool c::operator == (const data_t& other) const {
    if (size() != other.size()) return false;
    auto i = begin();
    auto j = other.begin();
    while (i != end()) {
        if (i->first != j->first) return false;
        if (i->second != j->second) return false;
        ++i;
        ++j;
    }
    return true;
}

bool c::operator != (const data_t& other) const {
    if (size() != other.size()) return true;
    auto i = begin();
    auto j = other.begin();
    while (i != end()) {
        if (i->first != j->first) return true;
        if (i->second != j->second) return true;
        ++i;
        ++j;
    }
    return false;
}

void c::dump(const string& pfx, ostream& os) const {
    lock_guard<mutex> lock(mx);
    dump_(pfx, os);
}

void c::dump_(const string& pfx, ostream& os) const {
    for (auto& i: *this) {
        os << pfx << "'" << i.first << "': " << i.second << '\n';
    }
}

c::diff_t c::get_diff(const data_t& prev) const {
    log("get_diff");
    auto newer = begin();
    auto older = prev.begin();
    diff_t d;
    while (newer != end() && older != prev.end()) {
        if (newer->first == older->first) {
            if (newer->second != older->second) {
                d.emplace(newer->first, diff_line{'M', newer->second});
            }
            ++newer;
            ++older;
            continue;
        }
        if (newer->first < older->first) {
            d.emplace(newer->first, diff_line{'A', newer->second});
            ++newer;
            continue;
        }
        d.emplace(older->first, diff_line{'D', older->second});
        ++older;
    }
    if (!empty()) {
        while (newer != end()) {
            d.emplace(newer->first, diff_line{'A', newer->second});
            ++newer;
        }
    }
    if (!prev.empty()) {
        while (older != prev.end()) {
            d.emplace(older->first, diff_line{'D', older->second});
            ++older;
        }
    }
    log("get_diff returning", d.size(), "lines");
    return move(d);
}

void c::diff_t::dump(const string& pfx, ostream& os) const {
    for (auto& i: *this) {
        os << pfx << i.second.action << ' ' << i.first << ' ' << i.second.value << '\n';
    }
}

void c::diff_t::filter(const string& k) {
    auto i = find(k);
    if (i != end()) {
        erase(i);
    }
}

ko c::apply(const diff_t& d) {
    for (auto& i: d) {
        switch (i.second.action) {
            case 'A': {
                if (!emplace(i.first, i.second.value).second) {
                    auto r = "KO 79658 item was already there.";
                    log(r);
                    return r;
                }
            }
            break;
            case 'M': {
                auto x = find(i.first);
                if (x == end()) {
                    auto r = "KO 79659 item was not found there.";
                    log(r);
                    return r;
                }
                x->second = i.second.value;
            }
            break;
            case 'D': {
                auto x = find(i.first);
                if (x == end()) {
                    auto r = "KO 79660 item was not found there.";
                    log(r);
                    return r;
                }
                erase(x);
            }
            break;
            default: {
                auto r = "KO 79661 unexpected action.";
                log(r);
                return r;
            }
        }
    }
    return ok;
}

ko c::from(const string& payload) {
    istringstream is(payload);
    return from(is);
}

ko c::from(istream& is) {
    log("data_t::from_stream");
    lock_guard<mutex> lock(mx);
    clear();
    while (is.good()) {
        string line;
        getline(is, line);
        if (line.empty()) continue;
        istringstream i2(line);
        string k;
        string v;
        i2 >> k;
        getline(i2, v);
        if (k.empty()) {
            auto r = "KO 66058";
            log(r);
            clear();
            return r;
        }
        if (is.fail()) {
            auto r = "KO 66059";
            log(r);
            clear();
            return r;
        }
        us::gov::io::cfg0::trim(v);
        emplace(k, v);
    }
    return ok;
}

c& c::operator = (const c& other) {
    b::operator = (other);
    return *this;
}

