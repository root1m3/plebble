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
#include "businesses_t.h"

#include <us/gov/config.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/bootstrap/protocols_t.h>
#include <us/wallet/engine/daemon_t.h>

#include "types.h"

#define loglevel "wallet/trader"
#define logclass "businesses_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::businesses_t;

c::businesses_t(wallet_local_api& parent): parent(parent) {
    log("businesses_t wallet home=", parent.home);
}

void c::init(const protocols_t& default_r2r) {
    log("init", default_r2r.size());
    ostringstream os;
    os << parent.home << "/protocols"; //.us/channel/wallet/guest/id/protocols;
    protocols_t available_protocols;
    auto r = available_protocols.load(os.str());
    if (is_ko(r)) {
        available_protocols = default_r2r;
    }
    log("plugins enabled on this wallet:", available_protocols.size());
    if (available_protocols.empty()) {
        log("No plugins are enabled on this custodial wallet.");
    }
    else {
        log("All plugins are enabled on this non-custodial wallet.");
    }
    for (auto& i: available_protocols) {
        log("Enabling protocol selection ", i.to_string2());
        auto r = emplace(i, nullptr);
        assert(r.second);
    }
    select_all();
}

c::~businesses_t() {
    log("destructor", this);
    assert(empty());
}

void c::cleanup() {
    log("cleanup");
    for (auto& i: *this) {
        log("cleanup", i.first.to_string2());
        parent.daemon.trades.libs.delete_business(i.second);
    }
    clear();
}

ko c::select(iterator& i) {
    log("select", i->first.to_string2());
    if (i->second != nullptr) {
        log("already selected", i->first.to_string2());
        return ok;
    }
    pair<ko, business_t*> r = parent.daemon.trades.libs.create_business(i->first);
    if (is_ko(r.first)) {
        assert(r.second == nullptr);
        return r.first;
    }
    assert(r.second != nullptr);
    i->second = r.second;
    i->second->init(parent.home + "/trader", protocol_factories);
    return ok;
}

void c::select_all() {
    for (auto i = begin(); i != end(); ++i) {
        if (i->second != nullptr) continue;
        log("selecting business for", i->first.to_string2());
        select(i);
    }
}

business_t* c::select(const protocol_selection_t& protocol_selection) {
    auto i = find(protocol_selection);
    if (i == end()) {
        return nullptr;
    }
    auto r = select(i);
    if (is_ko(r)) {
        return nullptr;
    }
    assert(i->second != nullptr);
    return i->second;
}

std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection) {
    log("create_protocol A", protocol_selection.to_string());
    return protocol_factories.create(protocol_selection);
}

/*
std::pair<ko, us::wallet::trader::trader_protocol*> c::create_opposite_protocolX(protocol_selection_t&& protocol_selection) {
    log("create_opposite_protocol A", protocol_selection.to_string());
    if (!protocol_selection.is_set()) {
        return make_pair(ok, nullptr);
    }
    for (auto& i: *this) {
        if (i.second == nullptr) continue;
        log("testing a business");
        auto r = i.second->create_opposite_protocol(protocol_selection_t(protocol_selection));
        if (r.first == ok) {
            log("found");
            return r;
        }
    }
    auto r = "KO 7895 Protocol not available";
    log(r);
    return make_pair(r, nullptr);
}
*/

std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection, params_t&& remote_params) {
    log("create_protocol B", protocol_selection.to_string());
    if (!protocol_selection.is_set()) {
        return make_pair(ok, nullptr);
    }
    auto r = protocol_factories.create(protocol_selection);
    if (is_ko(r.first)) {
        assert(r.second == nullptr);
        return r;
    }
    *r.second->remote_params = move(remote_params);
    return r;
}

void c::api_list_protocols(ostream& os) const {
    log("api_list_protocols");
    for (auto& i: *this) {
        i.first.api_list_protocols(os);
    }
}

void c::bookmark_info(vector<pair<protocol_selection_t, bookmark_info_t>>& o) const {
    for (auto& i: *this) {
        if (i.second == nullptr) continue;
        auto x = i.second->bookmark_info();
        //i.second->invert(x.first);
        o.emplace_back(x.first, x.second);
    }
}

void c::published_protocols(protocols_t& protocols, bool inverse) const {
    for (auto& i: *this) {
        i.second->published_protocols(protocols, inverse);
    }
}

/*
void c::invert(protocols_t& protocols) const {
    for (auto& i: *this) {
        i.second->invert(protocols);
    }
}

void c::invert(protocol_selection_t& protocol_selection) const {
    for (auto& i: *this) {
        i.second->invert(protocols);
    }
}
*/
ko c::invert(protocol_selection_t& protocol_selection) const {
    log("invert", protocol_selection.to_string2());
    for (auto& i: *this) {
        if (protocol_selection.first != i.first.first) { //exclude plugins not matching the protocol name
            continue;
        }
        if (i.second->invert(protocol_selection)) {
            log("inversion result:", protocol_selection.to_string2());
            return ok;
        }
    }
    auto r = "KO 60599 No plugin could be used to invert the role.";
    log(r);
    return r;
}


c::protocols_t c::published_protocols(bool inverse) const {
    protocols_t protocols;
    published_protocols(protocols, inverse);
    return move(protocols);
}

void c::dump(ostream& os) const {
    for (auto& i: *this) {
        os << i.first.to_string2() << ":\n";
        i.second->dump("  ", os);
    }
}

ko c::invert(qr_t& qr) {
    if (!qr.protocol_selection.is_set()) {
        return ok;
    }
    return invert(qr.protocol_selection);
}

pair<ko, hash_t> c::initiate(const hash_t parent_tid, const string& datadir, qr_t&& qr) {
    log("initiate new trade", qr.to_string());
    inverted_qr_t inverted_qr(move(qr));
    auto r = invert(inverted_qr);
    if (is_ko(r)) {
        return make_pair(r, hash_t());
    }
    log("initiate new trade. Inverted selection:", inverted_qr.to_string());
    if (inverted_qr.protocol_selection.is_set()) {
        auto i = find(inverted_qr.protocol_selection);
        if (i == end()) {
            auto r = "KO 57648 Protocol not available.";
            log(r, inverted_qr.to_string());
            return make_pair(r, hash_t());
        }
    }
    return parent.daemon.trades.initiate(parent_tid, datadir, move(inverted_qr), parent);
}

void c::recycle(business_t*) {
    log("reclycle bz");
}

void c::exec_help(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        ostringstream p;
        p << prefix << i.first.to_string2() << ' ';
        i.second->exec_help(p.str(), os);
    }
}

ko c::exec(istream& is) {
    log("libs_t::exec");
    string cmd;
    is >> cmd;
    protocol_selection_t o = protocol_selection_t::from_string2(cmd);
    auto i = find(o);
    if (i == end()) {
        auto r = "KO 85990 Library not found.";
        log(r);
        return r;
    }
    return i->second->exec(is, parent);
}

ko c::handle_r2r_index_hdr(protocols_t& protocols) {
    log("r2r_index_hdr");
    auto& demon = static_cast<engine::daemon_t&>(parent.daemon);
/*
    protocols.reserve(size());
    for (auto& i: *this) {
        assert(i.second != nullptr);
        protocol_selection_t ps = i.first;
        if (i.second->invert(ps)) {
            protocols.emplace(move(ps));
        }
    }
    return ok;
*/
    demon.bookmark_index.protocols([&](protocol_selection_t& candidate) {
        for (auto& i: *this) {
            if (i.second == nullptr) continue;
            protocol_selection_t ps = i.first;
            {
                auto r = i.second->invert(ps);
                assert(r);
            }
            if (ps != candidate) {
                continue;
            }
            candidate = i.first;
            return true;
        }
        return false;
    }, protocols);
    return ok;
}

ko c::handle_r2r_bookmarks(protocol_selection_t&& protocol_selection, bookmarks_t& bookmarks) {
    log("r2r_bookmarks");
    auto& demon = static_cast<engine::daemon_t&>(parent.daemon);
    auto i = find(protocol_selection);
    if (i == end()) {
        auto r = "KO 78643 I cannot run the selected protocol.";
        log(r);
        return r;
    }
    if (i->second->invert(protocol_selection)) {
        demon.bookmark_index.fill_bookmarks(protocol_selection, bookmarks);
    }
    return ok;
}

ko c::handle_r2r_index(bookmark_index_t& bookmark_index) {
    log("r2r_index");

    auto& demon = static_cast<engine::daemon_t&>(parent.daemon);
    bookmark_index = demon.bookmark_index.filter([&](const protocol_selection_t& ps) {
        return find(ps) != end();
    });
    return ok;
}

