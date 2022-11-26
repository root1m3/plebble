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
#include <sstream>

#include <us/gov/io/cfg1.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash/addresses_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/wallet/algorithm.h>
#include <us/wallet/wallet/tx_make_p2pkh_input.h>
#include <us/wallet/trader/bookmarks_t.h>

#include "types.h"
#include "local_api.h"

#define loglevel "wallet/wallet"
#define logclass "local_api"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::local_api;
using peer_t = us::wallet::engine::peer_t;

const char* c::KO_20183 = "KO 20183 Wallet contains no addresses.";

c::local_api(engine::daemon_t& daemon, const string& home, const string& subhome, const hash_t& subhomeh, trader::endpoint_t&& ep):
        daemon(daemon),
        home(home),
        subhome(subhome),
        subhomeh(subhomeh),
        device_filter([&](const socket::client& peer) -> bool {
                assert(static_cast<const peer_t&>(peer).local_w != nullptr);
                log("filter: peer subhome", static_cast<const peer_t&>(peer).local_w->subhome, "subhomeh", static_cast<const peer_t&>(peer).local_w->subhomeh, "==", this->subhomeh);
                return static_cast<const peer_t&>(peer).local_w->subhomeh == this->subhomeh;
            }),
        w(home + "/keys"),
        local_endpoint(ep),
        txlog(*this),
        businesses(*this),
        traders(*this) {

    businesses.init(daemon.trades.default_protocols(!subhome.empty()));
    bookmarks.init(home + "/trader");
    bookmarks.load();
    logdump("bookmarks> ", bookmarks);
}

c::~local_api() {
    businesses.cleanup();
}

ko c::refresh_data() {
    log("refresh_data");
    if (unlikely(empty())) {
        log("empty wallet. generating new_address.");
        algorithm::new_address();
    }
    if (unlikely(empty())) {
        log(KO_20183);
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = nullptr;
        return KO_20183;
    }
    us::gov::cash::addresses_t addresses;
    addresses.reserve(size());
    for (auto& i: *this) {
        addresses.emplace_back(i.first);
    }

    accounts_t* o = new accounts_t();
    auto r = daemon.gov_rpc_daemon.get_peer().call_fetch_accounts(addresses, *o);
    if (is_ko(r)) {
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = nullptr;
        return r;
    }
    {
        lock_guard<mutex> lock(w::mx);
        delete data;
        data = o;
    }
    return ok;
}

void c::dump(const string& pfx, ostream& os) const {
}

datagram* c::get_push_datagram(const hash_t& trade_id, uint16_t pc) const {
    auto blob = push_payload(pc);
    auto d = peer_t::push_in_t(trade_id, pc, blob).get_datagram(daemon.channel, 0);
    log("get_push_datagram", pc, "blob sz", blob.size(), "dgram sz", d->size());
    return d;
}

vector<datagram*> c::get_push_datagrams(const set<hash_t>& trade_ids, uint16_t pc) const {
    auto blob = push_payload(pc);
    vector<datagram*> v;
    v.reserve(trade_ids.size());
    for (auto& tid: trade_ids) {
        v.emplace_back(peer_t::push_in_t(tid, pc, blob).get_datagram(daemon.channel, 0));
    }
    return move(v);
}

blob_t c::push_payload(uint16_t pc) const {
    log("push_payload. push code", pc);
    assert(pc < push_end);
    blob_t blob;
    switch (pc) {
        case push_txlog: {
            log("computing push_txlog");
            txlog.index().write(blob);
            return move(blob);
        }
        break;
    }
    assert(false);
    return move(blob);
}

void c::on_tx_tracking_status(const track_status_t& status) {
    txlog.on_tx_tracking_status(status);
}

void c::published_bookmarks(bookmarks_t& b) const {
    log("published_bookmarks", b.size());
    using protocol_selection_t = us::wallet::trader::protocol_selection_t;
    using bookmark_info_t = us::wallet::trader::bookmark_info_t;
    using qr_t = us::wallet::trader::qr_t;

    int n = b.size();
    vector<pair<protocol_selection_t, bookmark_info_t>> v;
    businesses.bookmark_info(v);
    auto ep = local_endpoint;
    ep.wloc = subhome;
    for (auto& i: v) {
        ostringstream name;
        name << "bm_" << ++n;
        b.add(name.str(), bookmark_t(qr_t(ep, move(i.first)), move(i.second)));
        log("added", name.str());
    }
}

void c::published_bookmark_infos(vector<pair<protocol_selection_t, bookmark_info_t>>& v) const {
    log("published_bookmark_infos. before", v.size());
    businesses.bookmark_info(v);
    log("published_bookmark_infos. after", v.size());
}

ko c::exec(const string& cmd) {
    istringstream is(cmd);
    return exec(is);
}

ko c::exec(istream& is) {
    log("exec is");
    auto g = is.tellg();
    string cmd;
    is >> cmd;
    if (cmd.empty()) {
        ostringstream os;
        traders.exec_help(cmd, os);
        businesses.exec_help(cmd, os);
        push_OK(hash_t(0), os.str());
        return ok;
    }
//    ostringstream os;
/*
    if (cmd == "loc") { //handled by system program
        log("local_functions_exec");
        string cgidir = home + "/bin";
        string cmd;
        is >> cmd;
        if (cmd.empty()) {
            ostringstream os;
            os << "Available commands: \n";
            list_files_dir( "  ", cgidir, os );
            push_OK(hash_t(0), os.str(), w);
            return ok;
        }
        namespace fs = std::filesystem;
        for (auto& p: fs::directory_iterator(cgidir)) {
            if (!is_regular_file( p.path())) continue;
            if (p.path().filename().string() == cmd) {
                log("found file matching function name", cmd);
                string args;
                getline(is, args);
                ostringstream fcmd;
                fcmd << cgidir << '/' << cmd << ' ' << args;
                log("executing command", fcmd.str());
                ostringstream os;
                ko r = exec_shell(fcmd.str(), os);
                log("returned", (is_ko(r) ? r : "ok"));
                if (is_ok(r)) {
                    push_OK(hash_t(0), os.str(), w);
                }
                return r;
            }
        }
        log("KO 10478 Function ", cmd, "not found.");
        return "KO 10478 Invalid function.";
    }
*/
    is.seekg(g);
    auto r = traders.exec(is);
    if (r != traders_t::KO_39010) {
        return r;
    }
    return businesses.exec(is);
//    return "KO 10479 Invalid function.";
}

ko c::push_KO(ko msg) const {
    return daemon.pm.push_KO(msg, device_filter);
}

ko c::push_OK(const string& msg) const {
    return daemon.pm.push_OK(msg, device_filter);
}

ko c::push_KO(const hash_t& tid, ko msg) const {
    return daemon.pm.push_KO(tid, msg, device_filter);
}

ko c::push_OK(const hash_t& tid, const string& msg) const {
    return daemon.pm.push_OK(tid, msg, device_filter);
}

void c::schedule_push(socket::datagram* d) {
    return daemon.pm.schedule_push(d, device_filter);
}

void c::schedule_push(vector<socket::datagram*>&& d) {
    return daemon.pm.schedule_push(move(d), device_filter);
}

ko c::start() {
    return traders.start();
}

ko c::wait_ready(const time_point& deadline) const {
    return traders.wait_ready(deadline);
}

void c::stop() {
    traders.stop();
}

void c::join() {
    traders.join();
}

