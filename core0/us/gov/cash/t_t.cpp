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
#include "accounts_t.h"
#include "types.h"
#include "t_t.h"

#define loglevel "gov/cash"
#define logclass "t_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::t_t;

c::t_t() {
}

c::t_t(const t_t& other) {
    for (auto& i: other) {
        emplace(i);
    }
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

void c::hash_data(hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

void c::on_destroy(accounts_t& ledger) {
    for(auto& i: *this) {
        auto j = ledger.find(i.first);
        if (j != ledger.end()) {
            j->second.box.add(i.first, i.second);
        }
    }
}

void c::merge(const c& other) {
    for (auto i = other.begin(); i != other.end(); ++i) {
        auto j = emplace(*i);
        if (!j.second) {
            j.first->second += i->second;
        }
    }
}

void c::set_supply(const hash_t& token, const cash_t& supply) {
    auto i = find(token);
    if (i == end()) {
        if (likely(supply > 0)) {
            emplace(token, supply);
        }
    }
    else {
        if (supply > 0) {
            i->second = supply;
        }
        else {
            erase(i);
        }
    }
}

cash_t c::get_value(const hash_t& token) const {
    auto i = find(token);
    if (i == end()) return 0;
    return i->second;
}

bool c::burn(const hash_t& token, const cash_t&amount) {
    assert(token.is_not_zero());
    auto i = find(token);
    if (i == end()) return false;
    if (i->second >= amount) {
        i->second -= amount;
        if (i->second == 0) {
            erase(i);
        }
        return true;
    }
    return false;
}

void c::dump(const string& prefix, const hash_t& addr, ostream& os) const {
    for (auto& i: *this) {
        if (addr != i.first) {
            os << prefix << "coin " << i.first << " " << i.second << '\n';
        }
        else {
            os << prefix << "mint. Supply left " << i.second << '\n';
        }
    }
}

