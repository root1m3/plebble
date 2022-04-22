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
#include <functional>
#include <random>
#include <chrono>
#include <tuple>

#include <us/gov/config.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/auth/peer_t.h>

#include "acl_tx.h"
#include "db_t.h"
#include "local_delta.h"
#include "delta.h"
#include "node_address_tx.h"
#include "counters_t.h"
#include "types.h"

#define loglevel "gov/engine/auth"
#define logclass "app"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::app;

const char* c::KO_78101 = "KO 78101 Node not found.";
const char* c::KO_73291 = "KO 73291 Failed payload validation.";
const char* c::KO_31092 = "KO 31092 Unable to parse datagram.";

#if CFG_COUNTERS == 1
    counters_t c::counters;
#endif

c::app(engine::daemon_t& d): b(d), node_pubkey(d.id.pub), db(d.peerd.nodes, d.peerd.mx_nodes, d.peerd.hall, d.peerd.mx_hall), lock_nodes(d.peerd.mx_nodes, defer_lock), lock_hall(d.peerd.mx_hall, defer_lock) {
    pool = new engine::auth::local_delta();

}

c::~app() {
    delete pool;
}

us::gov::engine::peer_t::stage_t c::my_stage() const {
    if (cache_my_stage != peer_t::unknown) {
        return cache_my_stage;
    }
    auto k = node_pubkey.hash();
    unique_lock<mutex> lockn(db.mx_nodes);
    if (db.nodes.find(k) != db.nodes.end()) {
        cache_my_stage = peer_t::node;
        return cache_my_stage;
    }
    lockn.unlock();
    unique_lock<mutex> lockh(db.mx_hall);
    if (db.hall.find(k) != db.hall.end()) {
        cache_my_stage = peer_t::hall;
        return cache_my_stage;
    }
    lockh.unlock();
    cache_my_stage = peer_t::out;
    return cache_my_stage;
}

void c::basic_auth_completed(peer_t* p, pport_t rpport) {
    #if CFG_COUNTERS == 1
        ++counters.basic_auth_completed;
    #endif
    log ("node auth completed", p->endpoint(), p->pubkey.hash());
    if (p->is_role_sysop()) {
        if (p->pubkey == node_pubkey) {
            #if CFG_COUNTERS == 1
                ++counters.sysop_connections;
            #endif
            p->stage = peer_t::sysop;
            log ("stage is sysop");
        }
        else {
            auto r = "KO 93020 Unathorized public key.";
            log(r, p->pubkey, node_pubkey);
            p->stage = peer_t::unknown;
            p->disconnect(0, r);
        }
        return;
    }
    assert(p->is_role_peer());
    assert(!p->is_role_device());
    log("role node");
    if (!p->is_valid_ip(p->hostport.first, p->daemon.channel)) {
        #if CFG_COUNTERS == 1
            ++counters.invalid_net_address;
        #endif
        auto r = "KO 40020 Invalid net address.";
        log(r, "disconnect-invalid net address", p->endpoint());
        p->disconnect(0, r);
        return;
    }
    #ifdef CFG_TOPOLOGY_MESH
        log("Topology", "MESH");
        while(true) {
            {
                lock_guard<mutex> lock(db.mx_nodes);
                auto i = db.nodes.find(p->pubkey.hash());
                if (i != db.nodes.end()) {
                    p->stage = peer_t::node;
                    #if CFG_COUNTERS == 1
                        ++counters.node_connections;
                    #endif
                    log ("stage is node");
                    break;
                }
            }
            {
                lock_guard<mutex> lock(db.mx_hall);
                auto i = db.hall.find(p->pubkey.hash());
                if (i != db.hall.end()) {
                    p->stage = peer_t::hall;
                    #if CFG_COUNTERS == 1
                        ++counters.hall_connections;
                    #endif
                    log ("stage is hall");
                    break;
                }
            }
            log ("stage is out");
            #if CFG_COUNTERS == 1
                ++counters.out_connections;
            #endif
            p->stage = peer_t::out;
            break;
        }
        log("set stage", peer_t::stagestr[p->stage]);
        peer_t::stage_t newst;
        if (p->stage == peer_t::out){
            newst = peer_t::hall;
        }
        else {
            newst = p->stage;
        }
        log("creating tx node_address", p->pubkey.hash(), p->endpoint(), rpport);
        node_address_tx ev(p->pubkey.hash(), p->hostport.first, rpport);
        process(ev);
    #endif
    #ifdef CFG_TOPOLOGY_RING
        log("Topology","RING");
            auto i = db.nodes.find(p->pubkey.hash());
            if (i != db.nodes.end()) {
                p->stage = peer_t::node;
                #if CFG_COUNTERS == 1
                    ++counters.node_connections;
                #endif
                log ("stage is node");
            }
            else {
                #if CFG_COUNTERS == 1
                    ++counters.out_connections;
                #endif
                p->stage = peer_t::out;
                log ("stage is out");
            }
        log("set stage", peer_t::stagestr[p->stage]);
    #endif
}

bool c::process(const evidence& e) {
    log("process evidence", e.eid);
    if (e.eid == 0) {
        process(static_cast<const node_address_tx&>(e));
        return true;
    }
    #ifdef CFG_PERMISSIONED_NETWORK
        else if (e.eid == 1) {
            log("process evidence","acl_tx");
            process(static_cast<const acl_tx&>(e));
            return true;
        }
    #endif
    return false;
}

#ifdef CFG_PERMISSIONED_NETWORK
    pub_t c::node_master_pubkey(NODEMASTER_PUBKEY);

    bool c::account_state(const nodes_t& s, const hash_t& pubkeyh, host_t& netaddr, port_t& port) const {
        log("account_state from container");
        auto p = s.find(pubkeyh);
        if (likely(p == s.end())) return false;
        netaddr = p->second.net_address;
        port = p->second.port;
        log("found.", "netaddr", netaddr, "port", port);
        return true;
    }

    bool c::account_state(const auth::local_delta& w, const hash_t& pubkeyh, host_t& netaddr, port_t& port, peer_t::stage_t& stage) const {
        log("account_state from local_delta. nodes?");
        if (account_state(w.online, pubkeyh, netaddr, port)) {
            stage = peer_t::node;
            log("yes", "netaddr", netaddr, "port", port);
            return true;
        }
        log("account_state not found");
        return false;
    }

    bool c::account_state(const hash_t& pubkeyh, host_t& netaddr, port_t& port, peer_t::stage_t& stage) const {
        log("account_state from pubkeyh");
        log("acc state", "pool?");
        if (account_state(*pool, pubkeyh, netaddr, port, stage)) return true;
        log("acc state", "nodes?"); //TODO check if we need lock db
        if (account_state(db.nodes, pubkeyh, netaddr, port)) {
            stage = peer_t::node;
            log("yes", "netaddr", netaddr, "port", port);
            return true;
        }
        log("acc state", 3);
        log("acc state", "hall?");
        if (account_state(db.hall, pubkeyh, netaddr, port)) {
            stage = peer_t::hall;
            log("yes", "netaddr", netaddr, "port", port);
            return true;
        }
        log("acc state", "not found");
        return false;
    }

    bool c::process(const acl_tx& t) {
        log("process acl_tx evidence", t.addr, t.allow);
        if (t.pubkey != node_master_pubkey) {
            log("KO 40931 acl_tx: Invalid access.");
            return false;
        }
        ostream nullos(0);
        if (!t.verify(nullos)) {
            log("KO 40932 acl_tx: Invalid access. Failed verification");
            return false;
        }
        if (unlikely(t.addr.is_zero())) {
            log("KO 40933 acl_tx: Invalid address.");
            return false;
        }
        log("proc acl_tx", 1);
        account_t acc;
        peer_t::stage_t stage;
        peer_t::stage_t tstage = t.allow ? peer_t::node : peer_t::out;
        lock_guard<mutex> lock(mx_pool);
        if (account_state(t.addr, acc.net_address, acc.port, stage)) {
            log ("got account_state", acc.net_address, acc.port, peer_t::stagestr[stage]);
            if (stage == peer_t::node && tstage == peer_t::out) {
                log("proc acl_tx", 2);
                auto i = pool->online.find(t.addr);
                if (i != pool->online.end()) {
                    log("proc acl_tx", 3);
                    i->second.net_address = 0;
                }
                else {
                    log("proc acl_tx", 4);
                    acc.net_address = 0;
                    pool->online.emplace(t.addr, acc);
                }
                return true;
            }
            else if (stage == peer_t::hall && tstage == peer_t::out) {
                log("proc acl_tx", 5);
                return true;
            }
            else if (stage == peer_t::hall && tstage == peer_t::node) {
                log("proc acl_tx", 8);
                auto i = pool->online.find(t.addr);
                if (i != pool->online.end()) {
                    log("unexpected acl_tx", 10);
                    i->second = acc;
                }
                else {
                    log("proc acl_tx", 11);
                    pool->online.emplace(t.addr, acc);
                }
                return true;
            }
            else {
                log("proc acl_tx", 13);
                return false;
            }
        }
        else {
            log("proc acl_tx", 14);
            return false;
        }
    }
#endif

#ifdef CFG_PERMISSIONLESS_NETWORK
    void c::layoff(nodes_t& n, uint16_t cut) {
        log("layoff", cut);
        for (auto i = n.begin(); i != n.end(); ) {
            if (i->second.seen <= cut) {
                log("laid off ", i->first, i->second.seen, "<=", cut);
                i = n.erase(i);
            }
            else {
                ++i;
            }
        }
    }

    void c::layoff() {
        using namespace chrono;
        auto block_ts = demon.db->last_delta_imported_id;
        if (block_ts == 0) {
            block_ts = duration_cast<nanoseconds>(duration_cast<minutes>(system_clock::now().time_since_epoch())).count();
        }
        uint16_t cut = duration_cast<hours>(chrono::nanoseconds(block_ts)).count() / 24 - layoff_days;
        log("cut", cut);
        {
            log("lay off hall");
            lock_guard<mutex> lock(db.mx_hall);
            layoff(db.hall, cut);
        }
        {
            log("lay off nodes");
            lock_guard<mutex> lock(db.mx_nodes);
            layoff(db.nodes, cut);
        }
    }

    void c::add_growth_transactions(unsigned int seed) {
        log("add growth tx. seed", seed, "growth", growth, "min_growth", min_growth);
        if (abs(growth) < 1e-8) {
            log("growth is nearly 0", growth);
            return;
        }
        default_random_engine generator(seed);
        nodes_t* src;
        nodes_t* dst;
        size_t maxr;
        int s;
        if (growth >= 0) {
            size_t nh;
            {
                lock_guard<mutex> lock(db.mx_hall);
                nh = db.hall.size();
                if (nh == 0) {
                    log("empty hall");
                    return;
                }
            }
            s = floor((double) nh * growth);
            src = &db.hall;
            dst = &db.nodes;
            if (s < min_growth) s = min_growth;
            if (s > nh) s = nh;
            maxr = nh - 1;
            log("src=hall", "dst=nodes", "nh=", nh, "s=", s, "maxr=", maxr);
            lock_guard<mutex> lock(db.mx_nodes);
            if (db.nodes.size() >= CFG_MAX_NODES) {
                log("Reached limit of", db.nodes.size(), "/", CFG_MAX_NODES, "nodes. Keeping", nh, "nodes in hall.");
                return;
            }
        }
        else {
            size_t nn;
            {
                lock_guard<mutex> lock(db.mx_nodes);
                nn = db.nodes.size();
                if (nn == 0) {
                    return;
                }
            }
            s = -floor((double) nn * growth);
            src = &db.nodes;
            dst = &db.hall;
            maxr = nn - 1;
            log("src=nodes", "dst=hall", "nn=", nn, "s=", s, "maxr=", maxr);
        }
        uniform_int_distribution<size_t> distribution(0, maxr);
        lock(lock_nodes, lock_hall);
        logdump("GT before src>", *src);
        logdump("GT before dst>", *dst);
        for (size_t i = 0; i < s; ++i) {
            log("i", i, "src sz", src->size());
            auto p = src->begin();
            size_t r;
            while (true) {
                r = distribution(generator);
                if (r >= src->size()) continue;
                break;
            }
            advance(p, r);
            log("moving ", p->first);
            dst->emplace(*p);
            src->erase(p);
        }
        logdump("GT after src>", *src);
        logdump("GT after dst>", *dst);
        lock_nodes.unlock();
        lock_hall.unlock();        
    }
#endif

void c::process(const node_address_tx& t) {
    log("process node_address evidence.", t.pkh);
    logdump("  node_address::", t);
    if (unlikely(t.pkh.is_zero())) {
        log("KO 33029 invalid address.");
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
        pool->online.emplace(t.pkh, account_t(t.net_addr, t.pport, 0, 1));
    }
}

void c::import(const nodes_t& online) {
    log("import", online.size());
    using namespace chrono;
    auto block_ts = demon.db->last_delta_imported_id;
    if (block_ts == 0) {
        block_ts = duration_cast<nanoseconds>(duration_cast<minutes>(system_clock::now().time_since_epoch())).count();
    }
    auto seen = duration_cast<chrono::hours>(chrono::nanoseconds(block_ts)).count() / 24;
    log("block_ts", block_ts, "seen", seen);
    //unique_lock<mutex> lock1(db.mx_hall, defer_lock), lock2(db.mx_nodes, defer_lock);
    lock(lock_nodes, lock_hall);
    log("nn", db.nodes.size(), "nh", db.hall.size());
    for (auto& i: online) {
        if(unlikely(i.second.net_address == 0)) {
            log("ignoring entry with addr 0");
            continue;
        }
        auto n = db.nodes.find(i.first);
        if (n != db.nodes.end()) {
            if (i.second.net_address != n->second.net_address) {
                collusion_control.rm(n->second.net_address);
                if (likely(collusion_control.allow(i.second.net_address))) {
                    n->second = i.second;
                    n->second.seen = seen;
                    log("updated node address", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
                }
                else {
                    log("updated node address would cause collusion. deleting node.", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
                    db.nodes.erase(n);
                }
            }
            else {
                n->second = i.second;
                n->second.seen = seen;
                log("updated node", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
            }
            continue;
        }
        auto h = db.hall.find(i.first);
        if (h != db.hall.end()) {
            if (i.second.net_address != h->second.net_address) {
                collusion_control.rm(h->second.net_address);
                if (likely(collusion_control.allow(i.second.net_address))) {
                    h->second = i.second;
                    h->second.seen = seen;
                    log("updated halee address", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
                }
                else {
                    log("updated halee address would cause collusion. deleting halee.", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
                    db.hall.erase(h);
                }
            }
            else {
                h->second = i.second;
                h->second.seen = seen;
                log("updated hallee", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
            }
        }
        else {
            if (likely(collusion_control.allow(i.second.net_address))) {
                h = db.hall.emplace(i).first;
                h->second.seen = seen;
                log("added hall address", i.first, us::gov::socket::client::endpoint(i.second.net_address, i.second.port), seen);
            }
            else {
                log("collusion control blocked", i.first, i.second.net_address, us::gov::socket::client::endpoint(i.second.net_address, i.second.port));
            }
        }
    }
    lock_nodes.unlock();
    lock_hall.unlock();
}

void c::import(const engine::app::delta& gg, const engine::pow_t&) {
    #ifdef CFG_TOPOLOGY_MESH
        log("import delta");
        const engine::auth::delta& g = static_cast<const engine::auth::delta&>(gg);
        import(g.online);
        #ifdef CFG_PERMISSIONED_NETWORK
        #endif
        #ifdef CFG_PERMISSIONLESS_NETWORK
            layoff();
            add_growth_transactions(demon.rng_seed());
        #endif
    #endif
    cache_my_stage = peer_t::unknown;
}

bool c::report_node(const hash_t& pkh) {
    #ifdef CFG_TOPOLOGY_MESH
        log("report_node");
        //peer_t::stage_t st=peer_t::out;
        host_t net_address;
        port_t port;
        while(true) {
            {
                lock_guard<mutex> lock(db.mx_nodes);
                auto i = db.nodes.find(pkh);
                if (i != db.nodes.end()) {
                    net_address = i->second.net_address;
                    port = i->second.port;
                    log ("stage is node", us::gov::socket::client::endpoint(net_address, port));
                    break;
                }
            }
            {
                lock_guard<mutex> lock(db.mx_hall);
                auto i = db.hall.find(pkh);
                if (i != db.hall.end()) {
                    #if CFG_COUNTERS == 1
                        ++counters.hall_connections;
                    #endif
                    net_address = i->second.net_address;
                    port = i->second.port;
                    log ("stage is hall", us::gov::socket::client::endpoint(net_address, port));
                    break;
                }
            }
            log ("stage is out. not reporting");
            return false;
        }
        log ("creating tx node_address", pkh, net_address, port);
        node_address_tx ev(pkh, net_address, port);
        process(ev);
    #endif
    return true;
}

us::gov::engine::app::local_delta* c::create_local_delta() {
    lock_guard<mutex> lock(mx_pool);
    auto p = pool;
    pool = new engine::auth::local_delta();
    return p;
}

void c::clear() {
    db.clear();
    collusion_control.clear();
}

ko c::shell_command(istream& is, ostream& os) {
    while (is.good()) {
        string cmd;
        is >> cmd;
        if (cmd.empty()) {
            return ok;
        }
        if (cmd == "h" || cmd == "help" || cmd.empty()) {
            os << "Auth app shell.\n";
            os << "h|help                   Shows this help.\n";
            os << "db                       Lists nodes and candidates.\n";
            os << "seeds                    Dump db as seeds.\n";
            os << "ip4                      Lists unique ip4 addresses and number of nodes behind.\n";
            os << "lookup <pubkeyh>         Resolve Node IP:port from public-key-hash.\n";
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
            os << "I am " << peer_t::stagestr[my_stage()] << '\n';
            continue;
        }
        if (cmd == "nodes") {
            demon.peerd.dump_random_nodes(100, os);
            continue;
        }
        if (cmd == "ip4") {
            collusion_control.dump(os);
            continue;
        }
        if (cmd == "seeds") {
            db.dump_as_seeds(os);
            continue;
        }
        if (cmd == "lookup" || cmd == "lookup_node") {
            hash_t p;
            is >> p;
            auto r=db.lookup(p);
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
        return "KO 92817 Unrecognized command";
    }
    return ok;
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    db.dump(prefix, os);
}

size_t c::blob_size() const {
    lock(lock_nodes, lock_hall);
    return blob_writer_t::blob_size(db); //nodes & hall must be locked by the caller
}

void c::to_blob(blob_writer_t& writer) const {  //nodes & hall must be locked by the caller
    writer.write(db);
    lock_nodes.unlock();
    lock_hall.unlock();
}

ko c::from_blob(blob_reader_t& reader) {
    clear();
    {
        auto r = reader.read(db);
        if (is_ko(r)) return r;
    }
    {
        lock_guard<mutex> lock(db.mx_nodes);
        collusion_control.update_(db.nodes);
    }
    {
        lock_guard<mutex> lock(db.mx_hall);
        collusion_control.update_(db.hall);
    }
    cache_my_stage = peer_t::unknown;
    return ok;
}

pair<ko, us::gov::engine::evidence*> c::create_evidence(eid_t evid) {
    switch (evid) {
        case node_address_tx::eid: return make_pair(ok, new node_address_tx());
        #ifdef CFG_PERMISSIONED_NETWORK
        case acl_tx::eid: return  make_pair(ok, new acl_tx());
        #endif
    }
    auto r = "KO 65096 Invalid evidence id in engine/auth app.";
    log(r);
    return make_pair(r, nullptr);
}

