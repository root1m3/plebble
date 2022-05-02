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
#include "app.h"
#include "f_t.h"
#include "m_t.h"
#include "t_t.h"
#include "types.h"

#define loglevel "gov/cash"
#define logclass "accounts_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::accounts_t;

const char* c::KO_69213 = "KO 69213 Address not found.";

c::accounts_t() {
}

c::~accounts_t() {
}

account_t c::merge() const {
    account_t acc;
    for (auto& i: *this) {
        acc.merge(i.second);
    }
    return move(acc);
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    if (detail > 1) {
        os << prefix << size() << " accounts:\n";
    }
    for (auto& i: *this) {
        i.second.dump(prefix + "    ", i.first, detail, os);
    }
}

void c::list_files(const hash_t& addr, const string& path, ostream& os) const {
    auto i = find(addr);
    if (i == end()) {
        os << "KO 2991 Address " << addr << " not found.\n";
        return;
    }
    i->second.list_files(path, os);
}

hash_t c::file_hash(const hash_t& addr, const string& path) const {
    auto i = find(addr);
    if (i == end()) {
        return hash_t(0);
    }
    return i->second.file_hash(path);
}

ko c::print_data(const hash_t& addr, ostream& os) const {
    auto i = find(addr);
    if (i == end()) {
        return KO_69213;
    }
    i->second.print_data(addr, os);
    return ok;
}

void c::print_kv_b64(const hash_t& addr, ostream& os) const {
    auto i = find(addr);
    if (i == end()) {
        return;
    }
    i->second.box.print_kv_b64(os);
}

bool c::pay(const hash_t& address, const cash_t& amount) {
    if (amount <= 0) return false;
    auto i = find(address);
    if (i == end()) {
        emplace(make_pair(address, account_t{app::locking_program_id, amount}));
    }
    else {
        i->second.box.value += amount;
    }
    return true;
}

bool c::withdraw(const hash_t& address, const cash_t& amount) {
    auto i = find(address);
    if (i == end()) {
        return false;
    }
    else if (i->second.box.value < amount) {
        return false;
    }
    i->second.box.value -= amount;
    if (unlikely(i->second.box.value == 0)) {
        erase(i);
    }
    return true;
}

cash_t c::sum() const {
    cash_t s{0};
    for (auto& i: *this) {
        s += i.second.box.value;
    }
    return s;
}

void c::get_files(vector<pair<hash_t, pair<hash_t, fileattr_t>>>& v) const {
    for (auto& i: *this) {
        if (likely(i.second.box.f == nullptr)) continue;
        i.second.box.f->get_files(i.first, v);
    }
}

void c::get_files(vector<pair<hash_t, uint32_t>>& v) const {
    for (auto& i: *this) {
        if (likely(i.second.box.f == nullptr)) continue;
        i.second.box.f->get_files(v);
    }
}

void c::get_files(vector<pair<hash_t, fileattr_t>>& v) const {
    for (auto& i: *this) {
        if (likely(i.second.box.f == nullptr)) continue;
        i.second.box.f->get_files(v);
    }
}

void c::get_files(set<hash_t>& v) const {
    for (auto& i: *this) {
        if (likely(i.second.box.f == nullptr)) continue;
        i.second.box.f->get_files(v);
    }
}


void c::add(const batch_t& batch) {
    for (auto& b: batch) {
        auto i = find(b.first);
        if (likely(i == end())) {
            emplace(b);
        }
        else {
            i->second = b.second;
        }
    }
}

cash_t c::get_balance() const {
    cash_t b = 0;
    for (auto& i: *this) {
        b += i.second.box.value;
    }
    return move(b);
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

void c::batch_t::add(const hash_t& address, const batch_item& bi) {
    auto i = find(address);
    if (likely(i == end())) {
        emplace(address, bi);
    }
    else {
        i->second = bi;
    }
}

