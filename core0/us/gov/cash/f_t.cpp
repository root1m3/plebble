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
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#include "types.h"
#include "f_t.h"

#define loglevel "gov/cash"
#define logclass "f_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::f_t;

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        i.second.hash_data_to_sign(h);
    }
}

void c::hash_data(crypto::ripemd160& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        i.second.hash_data(h);
    }
}

void c::merge(const c& other) {
    for (auto i = other.begin(); i != other.end(); ++i) {
        emplace(*i);
    }
}

c::f_t() {
}

c::f_t(const f_t& other) {
    for (auto& i: other) {
        emplace(i);
    }
}

uint32_t c::total_mib() const {
    uint32_t n{0};
    for (auto& i: *this) {
        n += i.second.sz;
    }
    return n / (1024 * 1024);
}

void c::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << i.first << ' ';
        i.second.dump(os);
    }
}

hash_t c::file_hash(const string& path0) const {
    auto path = us::gov::io::cfg0::rewrite_path(path0);
    for (auto& i: *this) {
        if (i.second.path != path) continue;
        return i.first;
    }
    return move(hash_t(0));
}

void c::list_files(const string& path0, ostream& os) const {
    auto path = us::gov::io::cfg0::rewrite_path(path0);
    for (auto& i: *this) {
        if (i.second.path != path) continue;
        os << i.first << '\n';
    }
}

void c::get_files(vector<pair<hash_t, fileattr_t>>& v) const {
    v.reserve(v.size() + size());
    for (auto& i: *this) {
        v.emplace_back(i);
    }
}

void c::get_files(vector<pair<hash_t, uint32_t>>& v) const {
    v.reserve(v.size() + size());
    for (auto& i: *this) {
        v.emplace_back(make_pair(i.first, i.second.sz));
    }
}

void c::get_files(const hash_t&a, vector<pair<hash_t, pair<hash_t, fileattr_t>>>& v) const {
    v.reserve(v.size() + size());
    for (auto& i: *this) {
        v.emplace_back(make_pair(a, i));
    }
}

void c::get_files(set<hash_t>& v) const {
    for (auto& i: *this) {
        v.emplace(i.first);
    }
}

void c::diff(const f_t& newer, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files) const {
    const_iterator o = begin();
    const_iterator n = newer.begin();
    while(true) {
        if (o == end()) {
            new_files.reserve(new_files.size() + newer.size());
            for (; n != newer.end(); ++n) {
                new_files.emplace_back(make_pair(n->first, n->second.sz));
            }
            break;
        }
        if (n == end()) {
            old_files.reserve(old_files.size() + size());
            for (; o != end(); ++o) {
                old_files.emplace_back(make_pair(o->first, o->second.sz));
            }
            break;
        }
        if (o->first < n->first) {
            old_files.emplace_back(make_pair(o->first, o->second.sz));
        }
        else if (o->first > n->first) {
            new_files.emplace_back(make_pair(n->first, n->second.sz));
        }
        ++o; ++n;
    }
}

void c::delete_path(const string& path) {
    log("delete_path", path);
    for (auto i = begin(); i != end();) {
        if (i->second.path == path) {
            log("found+erased file", i->first);
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

void c::del_dup() {
    set<string> seen;
    for (auto i = begin(); i != end();) {
        auto x = seen.find(i->second.path);
        if (x != seen.end()) { //path seen
            i = erase(i);
        }
        else {
            seen.insert(i->second.path);
            ++i;
        }
    }
}

