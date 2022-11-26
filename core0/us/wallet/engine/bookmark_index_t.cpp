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
#include <us/gov/config.h>
#include <us/wallet/wallet/local_api.h>

#include "daemon_t.h"
#include "bookmark_index_t.h"

#define loglevel "wallet/engine"
#define logclass "bookmark_index_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet::engine;
using c0 = us::wallet::engine::bookmark_index_t;
using c = us::wallet::engine::bookmark_index2_t;

void c0::dump(ostream& os) const {
    for (auto& i: *this) {
        os << i.first.to_string2() << ":\n";
        i.second.dump("    ", os);
    }
}

/*
c0& c0::operator = (const c0& other) {
    clear();
    
}
*/
void c0::protocols(protocols_t& o) const {
    assert(o.empty());
    o.reserve(size());
    for (auto& i: *this) {
        o.emplace_back(i.first);
    }
}

void c0::protocols(function<bool (protocol_selection_t&)> enabled, protocols_t& o) const {
    assert(o.empty());
    o.reserve(size());
    for (auto& i: *this) {
        auto ps = i.first;
        if (!enabled(ps)) continue;
        o.emplace_back(ps);
    }
}

void c0::fill_bookmarks(const protocol_selection_t& protocol_selection, bookmarks_t& bookmarks) {
    auto i = find(protocol_selection);
    if (i == end()) {
        return;
    }
    bookmarks = i->second;
}

c0 c0::filter(function<bool (const protocol_selection_t&)> enabled) {
    c0 o;
    for (auto& i: *this) {
        if (!enabled(i.first)) continue;
        o.emplace(i);
    }
    return o;
}

void c::protocols(protocols_t& o) const {
    lock_guard<mutex> lock(mx);
    return b::protocols(o);
}

void c::protocols(function<bool (protocol_selection_t&)> enabled, protocols_t& o) const {
    lock_guard<mutex> lock(mx);
    return b::protocols(enabled, o);
}

void c::fill_bookmarks(const protocol_selection_t& protocol_selection, bookmarks_t& bookmarks) {
    lock_guard<mutex> lock(mx);
    b::fill_bookmarks(protocol_selection, bookmarks);
}

c0 c::filter(function<bool (const protocol_selection_t&)> enabled) {
    lock_guard<mutex> lock(mx);
    return b::filter(enabled);
}

c::bookmark_index2_t(daemon_t& d): parent(d) {
    lock_guard<mutex> lock(mx);
    load(filename());
}

string c::filename() const {
    ostringstream os;
    os << parent.home << "/bookmark_index";
    return os.str();
}

void c::add(wallet::local_api& w) {
    using bookmark_info_t = us::wallet::trader::bookmark_info_t;

    lock_guard<mutex> lock(mx);
    auto sz = size();

    vector<pair<protocol_selection_t, bookmark_info_t>> v;
    w.published_bookmark_infos(v);

    bookmarks_t bm;
    auto ep = w.local_endpoint;
    ep.wloc = w.subhome;
    int n = v.size();
    for (auto& i: v) {
        ostringstream name;
        name << "bm_" << ++n;
        bm.add(name.str(), bookmark_t(qr_t(ep, move(i.first)), move(i.second)));
        log("added", name.str());
    }
//    w.published_bookmarks(bm);
    if (bm.empty()) {
        return;
    }
    for (auto& i: bm) {
        auto j = find(i.second.qr.protocol_selection);
        if (j == end()) {
            j = emplace(i.second.qr.protocol_selection, bookmarks_t()).first;
        }
        ++sz;
        ostringstream name;
        name << "#" << sz;
        j->second.add(name.str(), move(i.second));
    }
    save(filename());
}

