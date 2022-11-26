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
#include <dlfcn.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <array>
#include <filesystem>

#include <us/gov/config.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/cash/ttx.h>
#include <us/gov/cash/map_tx.h>
#include <us/gov/cash/file_tx.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/r2r/w2w/business.h>

#include "trader_protocol.h"
#include "trader_t.h"
#include "business.h"
#include "businesses_t.h"
#include "bootstrap/initiator_t.h"
#include "bootstrap/follower_t.h"
#include "bootstrap/a_t.h"
#include "bootstrap/b_t.h"
#include "types.h"
#include "trades_t.h"

#define loglevel "wallet/trader"
#define logclass "trades_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::trades_t;

//home: .us/chan/wallet/trader"
c::trades_t(engine::daemon_t& daemon): daemon(daemon), libs(*this) {
    log("trades_t constructor");
//    us::gov::io::cfg0::ensure_dir(home);
    libs.load();
    //load_state();
    log(size(), "active wallets");
}

c::~trades_t() {
    log("destructor trades");
}

void c::save_default_protocols() const {
    auto root_wallet_home = daemon.wallet_home("");
    {
        auto protocols = default_protocols(false);
        ostringstream os;
        os << root_wallet_home << "/protocols.sysop"; //.us/channel/wallet/guest/id/protocols;
        log("saving available protocols for root wallets in", os.str());
        protocols.save(os.str());
    }
    {
        auto protocols = default_protocols(true);
        ostringstream os;
        os << root_wallet_home << "/protocols.guest"; //.us/channel/wallet/guest/id/protocols;
        log("saving available protocols for guest wallets in", os.str());
        protocols.save(os.str());
    }
}

c::protocols_t c::default_protocols(bool guest) const {
    if (guest) {
        return protocols_t{{"w2w", "w"}};
    }
    return libs.available_protocol_selections();
}

pair<ko, hash_t> c::initiate(const hash_t parent_tid, const string& datadir, inverted_qr_t&& inverted_qr, wallet::local_api& w) {
    auto r = w.traders.initiate(parent_tid, datadir, move(inverted_qr));
    if (is_ko(r.first)) {
        return r;
    }
    log("added route trade_id -> wallet/trader. route", 0, "subhome", w.subhome, "trade_id", r.second);
    lock_guard<mutex> lock(_mx);
    auto i = emplace(r.second, route_w(0, &w));
    assert(i.second);
    return r;
}

ko c::resume_trade(peer_t& peer, route_t route, const hash_t& trade_id, svc_t svc, const blob_t& blob) {
    switch(svc) { //follower instantiation
        case trader_t::svc_handshake_a1:  //without protocol_selection
        case trader_t::svc_handshake_c1: { //with protocol_selection
            log("resume trade as follower", svc);
            if (trade_id.is_zero()) {
                ko r = "KO 32010 Invalid trade id.";
                peer.disconnect(0, r);
                return r;
            }
            auto subhome = bootstrap::bootstrapper_t::extract_wloc(svc, blob);
            if (is_ko(subhome.first)) {
                peer.disconnect(0, subhome.first);
                return subhome.first;
            }
            log("subhome from handshake:", subhome.second); 
            if (!daemon.has_home(subhome.second)) {
                ko r = "KO 32913 wallet does not exist.";
                peer.disconnect(0, r);
                return r;
            }
            log("find local wallet. subhome", subhome.second);
            auto w = daemon.users.get_wallet(subhome.second); //local wallet used to trade against peer
            if (w == nullptr) {
                ko r = "KO 32912 Could not find wallet.";
                peer.disconnect(0, r);
                return r;
            }
            {
                log("find route to trader from trade_id", trade_id, "route", route);
                lock_guard<mutex> lock(_mx);
                auto i = find(trade_id);
                if (i == end()) {
                    i = emplace(trade_id, route_w(route, w)).first;
                    log("tid not found. Added entry in the routing table. tid", trade_id, "route", route, "subhome", w->subhome);
                }
                else {
                    log("finding entry in the routing table for tid", trade_id, "route", route, "subhome", w->subhome);
                    auto j = i->second.find(route);
                    if (unlikely(j != i->second.end())) {
                        ko r = "KO 77896. Unexpected. New trades cannot repeat route.";
                        peer.disconnect(0, r);
                        return r;
                    }
                    i->second.emplace(route, w);
                    log("added entry in the routing table. tid", trade_id, "route", route, "subhome", w->subhome);
                }
            }
            log("assigned local_wallet to trade tid", trade_id);
        }
        break;

        case trader_t::svc_handshake_a2:  //a == without protocol_selection
        //case trader_t::svc_handshake_b2:  //b == only protocol_selection
        case trader_t::svc_handshake_c2: { //c == with protocol_selection
            log("svc_handshake_2", svc);
            log("find route to trader from trade_id", trade_id);
            lock_guard<mutex> lock(_mx);
            auto i = find(trade_id);
            if (i == end()) {
                ko r = "KO 57847 The trade hasn't been initiated here.";
                assert(false);
                peer.disconnect(0, r);
                return r;
            }
            logdump("routing table> ", i->second);
            log("finding entry in the routing table for tid", trade_id);
            auto j = i->second.find(0);
            if (unlikely(j == i->second.end())) {
                auto r = "KO 77443. The trade hasn't been initiated here.";
                log(r);
                assert(false);
                peer.disconnect(0, r);
                return r;
            }
            auto w = j->second;
            i->second.erase(0);
            i->second.emplace(route, w);
            log("replaced route 0 with", route);
            log("added entry in the routing table. tid", trade_id, "route", route, "subhome", w->subhome);
        }
        break;
    }
    return ok;
}

void c::dump(const string& pfx, ostream& os) const {
    string pfx2 = pfx + "    ";
    for (auto& i: *this) {
        os << pfx << "tid " << i.first << '\n';
        i.second.dump(pfx2, os);
    }
}

void us::wallet::trader::route_w::dump(const string& pfx, ostream& os) const {
    for (auto& i: *this) {
        os << pfx << "route " << i.first << " subhome " << i.second->subhome << '\n';
    }
}

ko c::trading_msg(peer_t& peer, route_t route, const hash_t& trade_id, svc_t svc, blob_t&& blob) {
    log("trading_msg. svc", svc, "tid", trade_id, "route", route);
    assert(peer.is_role_peer());

    auto r = resume_trade(peer, route, trade_id, svc, blob);
    if (is_ko(r)) {
        return r;
    }

    log("resolve wallet");
    //resolve wallet. 1 peer attended by N wallets. (peer can be doing multiple trades simultaneously)
    //the payload goes to the wallet handling the trade_id
    wallet::local_api* w{nullptr};
    {
        log("find tid in routing table.", trade_id);
        lock_guard<mutex> lock(_mx);
        auto i = find(trade_id);
        if (i != end()) {
            log("find route in routing table. tid", trade_id, "route", route);
            auto j = i->second.find(route);
            if (j != i->second.end()) {
                log("found wallet. subhome", j->second->subhome);
                w = j->second;
            }
        }
    }
    if (unlikely(w == nullptr)) {
        logdump("routing table> ", *this);
        //I don't know what trade_id is this guy talking about.
        auto r = "KO 58763 I don't know the trade_id you are talking about.";
        log(r);
        assert(false);
        return r;
    }
    log("deliver msg to wallet");
    return w->traders.trading_msg(peer, svc, trade_id, move(blob)); //let wallet in charge handle
}

void c::reload_file(const string& fqn) {
    lock_guard<mutex> lock(_mx);
    for (auto& i: *this) {
        for (auto& j: i.second) {
            auto* w = j.second;
            if (fqn.size() < w->home.size()) continue;
            if (fqn.substr(0, w->home.size()) != w->home) continue;
            w->traders.reload_file(fqn);
        }
    }
}








/*
void c::load_lf() {
    namespace fs = std::filesystem;
    string cgidir = home + "/bin";
    us::gov::io::cfg0::ensure_dir(cgidir);
    for (auto& p: fs::directory_iterator(cgidir)) {
        if (!is_regular_file(p.path())) continue;
        lf.emplace(p.path().filename().string());
    }
}
*/

/*
void c::on_file_updated2(const string& path, const string& name, const trader_t* source_trader) { //shall be locked on caller
    log("on_file_updated2");
    lock_guard<mutex> lock(_mx);
    for (auto i = begin(); i != end(); ++i) {
        if (i->second == source_trader) continue;
        i->second->on_file_updated(path, name);
    }
    log("end of on_file_updated");
}
*/

/*
ko c::exec(istream& is, wallet::local_api& w) {
    log("exec is");
    auto g = is.tellg();
    string cmd;
    is >> cmd;
    if (cmd.empty()) {
        ostringstream os;
        exec_help(w, "", os);
        push_OK(hash_t(0), os.str(), w);
        return ok;
    }
    ostringstream os;
    if (cmd == "personality") {
        string cmd;
        is >> cmd;
        if (cmd == "set") {
            string sk;
            string moniker;
            is >> sk;
            getline(is, moniker);
            us::gov::io::cfg0::trim(moniker);
            if (is.fail()) {
                auto r = "KO 89979";
                log(r);
                return r;
            }
            auto r = personality.reset_if_distinct(sk, moniker);
            if (r == 0) {
                return push_OK(hash_t(0), "Personality didn't change.", w);
            }
            ostringstream os;
            os << "New trades will be initiated using personality ";
            personality.one_liner(os);
            os << '.';
            return push_OK(hash_t(0), os.str(), w);
        }
        auto r = "KO 50399 Invalid personality command.";
        log(r);
        return r;
    }
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
    is.seekg(g);
    return w.businesses.exec(is);
//    return "KO 10479 Invalid function.";
}

void c::reload_file(const string& fqn) {
    log("reload_file", fqn);
    auto r = us::gov::io::cfg0::split_fqn(fqn);
    if (is_ko(r.first)) {
        return;
    }
    if (r.second.second.empty()) {
        auto r = "KO 74477 Path to a directory given.";
        log(r);
        return;
    }
    on_file_updated2(r.second.first, r.second.second, nullptr);
}
*/
/*
void c::schedule_push(socket::datagram* d, wallet::local_api& w) {
    return daemon.pm.schedule_push(d, w.device_filter);
}


ko c::push_KO(ko msg, wallet::local_api& w) {
    return daemon.pm.push_KO(msg, w.device_filter);
}

ko c::push_OK(const string& msg, wallet::local_api& w) {
    return daemon.pm.push_OK(msg, w.device_filter);
}


ko c::push_KO(const hash_t& tid, ko msg, wallet::local_api& w) {
    return daemon.pm.push_KO(tid, msg, w.device_filter);
}

ko c::push_OK(const hash_t& tid, const string& msg, wallet::local_api& w) {
    return daemon.pm.push_OK(tid, msg, w.device_filter);
}
*/

/*
size_t c::blob_size() const {
    auto sz = blob_writer_t::sizet_size(size()) + size() * sizeof(uint64_t);
    for (auto& i: *this) {
        sz += blob_writer_t::blob_size(i.second->w->subhome);
    }
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    auto n = size();
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()), "list size", n);
    writer.write_sizet(n);
    for (auto& i: *this) {
        uint64_t ndx = i.first;
        writer.write(ndx);
        writer.write(i.second->w->subhome);
    }
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    uint64_t sz;
    auto r = reader.read_sizet(sz);
    if (is_ko(r)) return r;
    if (unlikely(sz > blob_reader_t::max_sizet_containers)) return blob_reader_t::KO_75643;
    for (int i = 0; i < sz; ++i) {
        log("loading trade #", i);
        uint64_t utid;
        {
            auto r = reader.read(utid);
            if (is_ko(r)) return r;
            log("read utid", utid);
        }
        string subhome;
        {
            auto r = reader.read(subhome);
            if (is_ko(r)) return r;
            log("read subhome", subhome);
        }
        {
            log("boot with no bootstrapper", utid, subhome);
            auto w = daemon.users.get_wallet(subhome);
            if (w == nullptr) {
                auto r = "KO 87868 wallet could not be instantiated.";
                log(r, "subhome", subhome);
                continue;
            }
            trader_t* tder = lock_trader_(utid);
            assert(tder != nullptr);
            auto r = tder->boot(utid, *w);
            log("--busyref", utid);
            --tder->busyref;
            if (is_ko(r)) {
                log(r, utid);
                erase_trader_(utid);
                continue;
            }
        }
    }
    return ok;
}

string c::sername() const {
    return home + "/active";
}

void c::load_state_() {
    log("loading active trades");
    auto r = load(sername());
    if (is_ko(r)) {
        log("active trades could not be loaded", sername());
    }
}

void c::load_state() {
    lock_guard<mutex> lock(_mx);
    load_state_();
}

void c::save_state_() const { //assume mx is taken
    log("saving active trades");
    auto r = save(sername() + "_off"); // TODO: re-enable trades persistence
    if (is_ko(r)) {
        log("active trades could not be saved", sername());
    }
}

void c::save_state() const { //assume mx is taken
    lock_guard<mutex> lock(_mx);
    save_state_();
}
*/

