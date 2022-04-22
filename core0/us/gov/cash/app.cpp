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
#include "app.h"

#include <random>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <us/gov/io/cfg0.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/protocol.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

#include "tx_t.h"
#include "ttx.h"
#include "map_tx.h"
#include "file_tx.h"
#include "local_delta.h"
#include "safe_deposit_box.h"
#include "delta.h"
#include "types.h"

#define loglevel "gov/cash"
#define logclass "app"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::app;

constexpr const char* c::name;

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c::app(engine::daemon_t& d): b(d) {
    pool = new c::local_delta();
}

c::~app() {
    delete pool;
}

engine::app::local_delta* c::create_local_delta() {
    lock_guard<mutex> lock(mx_pool);
    auto p = pool;
    pool = new c::local_delta();
    return p;
}

bool c::check_amounts(const tx_t& t) {
    if (!t.check_amounts()) {
        return false;
    }
    return true;
}

pair<ko, engine::evidence*> c::create_evidence(eid_t evid) {
    switch(evid) {
        case tx_t::eid: return make_pair(ok, new tx_t());
        case ttx::eid: return make_pair(ok, new ttx());
        case map_tx::eid: return make_pair(ok, new map_tx());
        case file_tx::eid: return make_pair(ok, new file_tx());
    }
    auto r = "KO 65094 Invalid evidence id in cash app.";
    log(r);
    return make_pair(r, nullptr);
}

cash_t c::pay_services() const { //state budgets
    //fill budgets
    return 0;
}

void c::tax_accounts(const cash_t& amount) {
}

void c::process_profit(cash_t& profit) {
    if (profit < db.max_subsidy) {
        profit = db.max_subsidy;
    }
    cash_t services_cost = pay_services();
    if (services_cost > profit) {
        tax_accounts(services_cost - profit);
        profit = 0;
    }
}

void c::pay_subsidy(const cash_t& amount, const engine::pow_t& w) {
    if (unlikely(w.empty())) return;
    double total_cash = (double)amount;
    uint64_t total_work = w.sum();
    cash_t paid = 0;
    lock_guard<mutex> lock(db.mx);
    for (auto& i: w) {
        double rel_work;
        if (likely(total_work > 0)) {
            rel_work = (double)i.second / (double)total_work;
        }
        else {
            rel_work = 1.0 / (double)w.size();
        }
        cash_t am = total_cash * rel_work;
        db.add_(i.first, am);
        paid += am;
    }
    cash_t remainder = amount - paid;
    if (remainder > 0) {
        default_random_engine generator(demon.rng_seed());
        uniform_int_distribution<size_t> distribution(0, w.size() - 1);
        auto i = w.begin();
        advance(i, distribution(generator));
        db.add_(i->first, remainder);
    }
}

cash_t c::charge_ledger() {
    cash_t fee{0};
    for (auto i = db.accounts->begin(); i != db.accounts->end(); ) {
        fee += i->second.box.charge(0);
        if (unlikely(i->second.box.value == 0)) {
            i->second.on_destroy(*db.accounts);
            i = db.accounts->erase(i);
        }
        else {
            ++i;
        }
    }
    return fee;
}

void c::update_ledger(const delta& g, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files) {
    const auto& a = g.g.accounts;
    lock_guard<mutex> lock(db.mx);
    for (auto& i: a) {
        auto d = db.accounts->find(i.first);
        if (d == db.accounts->end()) {
            if (likely(i.second.box.value != 0)) {
                db.accounts->emplace(i.first, account_t(i.second.locking_program, i.second.box.value));
            }
        }
        else {
            if (i.second.box.value == 0) {
                d->second.box.get_files(old_files);
                db.accounts->erase(d);
            }
            else {
                d->second.locking_program = i.second.locking_program;
                d->second.box.diff_files(i.second.box, old_files, new_files);
                d->second.box = i.second.box;
            }
        }
    }
}

void c::import(const engine::app::delta& gg, const engine::pow_t& w) {
    old_files.clear();
    new_files.clear();
    const delta& g = static_cast<const delta&>(gg);
    update_ledger(g, old_files, new_files);
    //CRYPTOECONOMY
    cash_t maint_fees = charge_ledger();
    cash_t profit = maint_fees;
    cash_t tx_fees = g.g.fees;
    profit += g.g.fees;
    process_profit(profit);
    pay_subsidy(profit, w);
}

bool c::account_state(const batch_t& batch, const hash_t& address, account_t& acc) const {
    auto b = batch.find(address);
    if (likely(b == batch.end())) {
        auto p = pool->accounts.find(address);
        if (likely(p == pool->accounts.end())) {
            lock_guard<mutex> lock(db.mx);
            auto o = db.accounts->find(address);
            if (o == db.accounts->end()) {
                return false;
            }
            acc.locking_program = o->second.locking_program;
            acc.box = o->second.box;
        }
        else {
            acc.locking_program = p->second.locking_program;
            acc.box = p->second.box;
        }
    }
    else {
        acc.locking_program = b->second.locking_program;
        acc.box = b->second.box;
    }
    return true;
}

void c::report_wallet(const tx_t& t, const string&) const {
}

bool c::process(const evidence& e) {
    if (e.eid == 0) {
        return process(static_cast<const tx_t&>(e));
    }
    else if (e.eid == 1) {
        return process(static_cast<const map_tx&>(e));
    }
    else if (e.eid == 2) {
        return process(static_cast<const file_tx&>(e));
    }
    else if (e.eid == 3) {
        return process(static_cast<const ttx&>(e));
    }
    return false;
}

ko c::shell_command(istream& is, ostream& os) {
    while(is.good()) {
        string cmd;
        is >> cmd;
        if (cmd.empty()) {
            return ok;
        }
        if (cmd == "h" || cmd == "help") {
            os << "cash shell.\n";
            os << "m|mempool           dumps mempool.\n";
            os << "db <0|1|2>          dumps db [detail level])\n";
            os << "files               dump public files\n";
            os << "exit                return to parent shell.\n";
            os << '\n';
            continue;
        }
        if (cmd == "m" || cmd == "mempool") {
            lock_guard<mutex> lock(mx_pool);
            pool->accounts.dump("", 2, os);
            os << "fee " << UGAS << ' ' << pool->fees << '\n';
            continue;
        }
        if (cmd == "db") {
            int detail = 0;
            is >> detail;
            db.dump("", detail, os);
            continue;
        }
        if (cmd == "files") {
            vector<pair<hash_t, fileattr_t>> f;
            get_files(f);
            for (auto& i: f) {
                os << i.first << ' ';
                i.second.dump(os);
            }
            os << f.size() << " files.\n";
            continue;
        }
        if (cmd == "exit") {
            return ok;
        }
        return "KO 50493 Unrecognized command";
    }
    return ok;
}

void c::clear() {
    db.clear();
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    db.dump(prefix, detail, os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(db);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(db);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(db);
        if (is_ko(r)) return r;
    }
    return ok;
}

void c::get_files(vector<pair<hash_t, fileattr_t>>& f) const {
    lock_guard<mutex> lock(db.mx);
    db.accounts->get_files(f);
}

void c::get_files(vector<pair<hash_t, uint32_t>>& f) const {
    lock_guard<mutex> lock(db.mx);
    db.accounts->get_files(f);
}

void c::get_files(set<hash_t>& f) const {
    lock_guard<mutex> lock(db.mx);
    db.accounts->get_files(f);
}

