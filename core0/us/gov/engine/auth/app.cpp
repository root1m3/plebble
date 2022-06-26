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

#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>

#include "local_delta.h"
#include "node_address_tx.h"
#include "db_t.h"
#include "delta.h"
#include "acl_tx.h"
#include "types.h"

#define loglevel "gov/engine/auth"
#define logclass "app"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::app;

const char* c::KO_78101 = "KO 78101 Node not found.";
const char* c::KO_73291 = "KO 73291 Failed payload validation.";
const char* c::KO_31092 = "KO 31092 Unable to parse datagram.";

#if CFG_TEST == 1
    size_t c::cfg_shard_size{CFG_SHARD_SIZE};
#endif

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
            #if CFG_ENABLE_SHARDING == 1
                shard();
            #endif
        #endif
    #endif
    cache_my_stage = peer_t::unknown;
}

bool c::report_node(const hash_t& pkh) {
    #ifdef CFG_TOPOLOGY_MESH
        log("report_node");
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

