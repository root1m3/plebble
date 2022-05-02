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
#include "app.h"

#include <functional>
#include <random>
#include <tuple>

#include <us/gov/config.h>
#include <us/gov/auth/peer_t.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>

#include "wallet_address.h"
#include "local_delta.h"
#include "delta.h"
#include "types.h"

#define loglevel "gov/traders"
#define logclass "app"
#include <us/gov/logs.inc>

using namespace us::gov::traders;
using c = us::gov::traders::app;

const char* c::KO_78101 = "KO 78101 Node not found.";
const char* c::KO_73291 = "KO 73291 Failed payload validation.";

c::app(engine::daemon_t& d): b(d) {
    pool = new traders::local_delta();
}

c::~app() {
    delete pool;
}

bool c::process(const evidence& e) {
    log("process evidence", e.eid);
    if (e.eid == wallet_address::eid) {
        process(static_cast<const wallet_address&>(e));
        return true;
    }
    return false;
}

void c::process(const wallet_address& t) {
    log("process wallet_address evidence.");
    logdump("  wallet_address::", t);
    if (unlikely(t.pkh.is_zero())) {
        auto r = "KO 34029 invalid address.";
        log(r);
        return;
    }
    lock_guard<mutex> lock(mx_pool);
    auto i = pool->online.find(t.pkh);
    if (i != pool->online.end()) {
        log("B111");
        i->second.net_address = t.net_addr;
        i->second.port = t.pport;
    }
    else {
        log("B112");
        pool->online.emplace(t.pkh, account_t(t.net_addr, t.pport, 0, 0));
    }
}

void c::import(const wallets_t& online) {
    using namespace std::chrono;
    log("import", online.size());
    auto block_ts = demon.db->last_delta_imported_id;
    if (block_ts == 0) {
        block_ts = duration_cast<nanoseconds>(duration_cast<minutes>(system_clock::now().time_since_epoch())).count();
    }
    auto seen = duration_cast<chrono::hours>(chrono::nanoseconds(block_ts)).count() / 24;
    log("block_ts", block_ts, "seen", seen);
    lock_guard<mutex> lock(db.mx_wallets);
    log("nw", db.wallets.size());
    for (auto& i: online) {
        if(unlikely(i.second.net_address == 0)) {
            log("ignoring entry with addr 0");
            continue;
        }
        auto w = db.wallets.find(i.first);
        if (w != db.wallets.end()) {
            w->second = i.second;
            w->second.seen = seen;
            log("updated wallet address", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
            continue;
        }
        w = db.wallets.emplace(i).first;
        w->second.seen = seen;
        log("added wallet address", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
    }
}

void c::import(const engine::app::delta& gg, const engine::pow_t&) {
    log("import delta");
    auto& g = static_cast<const traders::delta&>(gg);
    import(g.online);
}

us::gov::engine::app::local_delta* c::create_local_delta() {
    lock_guard<mutex> lock(mx_pool);
    auto p = pool;
    pool = new traders::local_delta();
    return p;
}

void c::clear() {
    db.clear();
}

ko c::shell_command(istream& is, ostream& os) {
    while (is.good()) {
        string cmd;
        is >> cmd;
        if (cmd.empty()) {
            return ok;
        }
        if (cmd == "h" || cmd == "help") {
            os << "Traders app shell.\n";
            os << "h|help                   Shows this help.\n";
            os << "db                       Lists endpoints.\n";
            os << "seeds                    Dump db as seeds.\n";
            os << "lookup <pubkeyh>         Resolve IP:port from public-key-hash.\n";
            os << "m|mempool                dumps mempool.\n";
            os << "exit                     Exits this app and returns to parent shell.\n";
            os << '\n';
            return ok;
        }
        if (cmd == "exit") {
            return ok;
        }
        if (cmd == "db") {
            db.dump("", os);
            continue;
        }
        if (cmd == "lookup") {
            hash_t p;
            is >> p;
            auto r = db.lookup(p);
            if (is_ko(r.first)) {
                os << r.first << '\n';
            }
            else {
                r.second.print_endpoint(os);
                os << '\n';
            }
            continue;
        }
        if (cmd == "m" || cmd == "mempool") {
            lock_guard<mutex> lock(mx_pool);
            pool->dump("", os);
            continue;
        }
        return "KO 70694 Unrecognized command.";
    }
    return ok;
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    db.dump(prefix, os);
}

pair<ko, engine::evidence*> c::create_evidence(eid_t evid) {
    switch (evid) {
        case wallet_address::eid: return make_pair(ok, new wallet_address());
    }
    auto r = "KO 65092 Invalid evidence id in traders app.";
    log(r);
    return make_pair(r, nullptr);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(db);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(db);
}

ko c::from_blob(blob_reader_t& reader) {
    clear();
    {
        auto r = reader.read(db);
        if (is_ko(r)) return r;
    }
    return ok;
}

