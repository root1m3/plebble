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
#include "daemon_t.h"
#include "protocol.h"
#include "peer_t.h"
#include <chrono>
#include <random>
#include <us/gov/io/cfg.h>

#define loglevel "gov/peer"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::peer;
using c = us::gov::peer::daemon_t;

us::gov::peer::mezzanine::mezzanine(daemon_t* d): d(d), b(bind(&c::run, d), bind(&c::on_t_wakeup, d)) {
}

c::daemon_t(channel_t channel): b(channel, 0, 0, 0), myself(0), t(this) {
}

c::daemon_t(channel_t channel, port_t port, pport_t pport, uint8_t dimensions, uint8_t edges, uint8_t devices, uint8_t workers): b(channel, port, pport, workers), myself(0), t(this) {
    assert(dimensions > 0);
    clique.resize(dimensions, edges);
    log("grid-peer clique size. dimensions", dimensions, "edges", (*clique.begin())->size());
    {
        lock_guard<mutex> lock(grid_dev.mx_);
        assert(grid_dev.empty());
        grid_dev.resize(devices, nullptr);
    }
    log("grid-devices size", grid_dev.size());
}

c::~daemon_t() {
}

void c::stop() {
    log("stop");
    b::stop();
    log("stopping grid daemon");
    t::stop();
    log("stopped grid daemon");
}

us::ko c::start() {
    log("start");
    myself = get_keys().pub.hash();
    if(myself.is_zero()) {
        auto r = "KO 40392 Invalid keys.";
        log(r);
        return r;
    }
    ko r = b::start();
    if (unlikely(r != ok)) {
        return r;
    }
    r = t::start();
    if (unlikely(r != ok)) {
        b::stop();
        return r;
    }
    log("started grid daemon");
    return ok;
}

us::ko c::wait_ready(const chrono::system_clock::time_point& deadline) const {
    log("wait_ready");
    auto r = b::wait_ready(deadline);
    if (unlikely(r != ok)) {
        return r;
    }
    return t::wait_ready(deadline);
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::join() {
    b::join();
    log("waiting for grid daemon");
    t::join();
    log("stopped");
}

void c::on_t_wakeup() {
    cv.notify_all();
}

vector<hostport_t> c::list_neighbours() const {
    return clique.list();
}

vector<hostport_t> c::list_dev_neighbours() const {
    vector<hostport_t> v;
    unique_lock<mutex> lock(grid_dev.mx_);
    for (auto& i: grid_dev) {
        if (i != nullptr) {
            v.emplace_back(i->hostport);
        }
    }
    return move(v);
}

void c::on_destroy_(socket::client& cli) {
    auto& peer = static_cast<peer_t&>(cli);
    if (clique.ended(&peer)) {
        log("slot destroyed in clique", &peer);
        t::reset_resume();
        cv.notify_all();
        return;
    }
    if (grid_dev.ended(&peer)) {
        log("slot destroyed in grid_dev", &peer);
    }
}

bool c::grid_connect_test(peer_t* p, const hostport_t& hostport, ostream& os) {
    os << "connecting to " << peer_t::endpoint(hostport) << " pport " << pport << '\n';
    auto r = p->connect(hostport, pport, 0, peer_t::role_peer, true);
    if (likely(r == ok)) {
        os << "connected\n";
        attach(p);
        return true;
    }
    delete p;
    return false;
}

ko c::grid_connect(const hostport_t& hostport, function<void(peer_t*)> pre_connect, function<void(peer_t*)> pre_attach) {
    auto* p = static_cast<peer_t*>(create_client(-1));
    pre_connect(p);
    auto r = p->connect(hostport, pport, 0, peer_t::role_peer, true);
    if (likely(r == ok)) {
        log("grid-connect", peer_t::endpoint(hostport), "pport", pport, p);
        pre_attach(p);
        attach(p);
        log("Attached to grid", peer_t::endpoint(hostport), "pport", pport, p);
        return ok;
    }
    faillog.add(hostport);
    log("grid-connect failed", peer_t::endpoint(hostport), "pport", pport, r);
    delete p;
    auto rf = "KO 80191 Connection failed.";
    log(rf);
    return rf;
}

void c::wait() {
    using namespace chrono;
    static mt19937_64 rng(random_device{}());
    static uniform_int_distribution<> d(10, 30);
    if (t::reset_wait()) {
        return;
    }
    seconds ws(d(rng));
    log("going to sleep for ", ws.count(), "secs");
    mutex mx;
    unique_lock<mutex> lock(mx);
    cv.wait_for(lock, ws, [&]{ return t::resumed(); });
    log("wakeup");
    t::reset_wait();
}

void c::set_seed_nodes(const vector<hostport_t>& sn) {
    log("set_seed_nodes", sn.size());
    lock_guard<mutex> lock(mx_seed_nodes);
    seed_nodes.clear();
    for (auto& i: sn) {
        add_seed_node__(i);
    }
}

void c::set_nodes(const vector<pair<hash_t, hostport_t>>& n) {
    log("set_nodes", n.size());
    lock_guard<mutex> lock(mx_nodes);
    nodes.clear();
    for (auto& i: n) {
        add_node__(i);
    }
}

ko c::add_seed_node(const hostport_t& i) {
    log("add_seed_node");
    lock_guard<mutex> lock(mx_seed_nodes);
    hash_t tempid(seed_nodes.size() + 1);
    account_t a(i.first, i.second, 0, 1);
    log("checking", tempid, i.first, i.second);
    ko r = nodes_t::check(tempid, a, channel);
    if (is_ko(r)) {
        return r;
    }
    log("adding to seed_nodes", tempid, i.first, i.second, peer_t::endpoint(i));
    seed_nodes.emplace(tempid, a);
    return ok;
}

ko c::add_seed_node__(const hostport_t& i) {
    log("add_seed_node_");
    hash_t tempid(seed_nodes.size() + 1);
    account_t a(i.first, i.second, 0, 1);
    log("checking", tempid, i.first, i.second);
    ko r = nodes_t::check(tempid, a, channel);
    if (is_ko(r)) {
        return r;
    }
    log("adding to seed_nodes", tempid, i.first, i.second, peer_t::endpoint(i));
    seed_nodes.emplace(tempid, a);
    return ok;
}

ko c::add_node__(const pair<hash_t, hostport_t>& i) { //nodes locked by caller
    log("add_node_");
    account_t a(i.second.first, i.second.second, 0, 1);
    log("checking", i.first, i.second.first, i.second.second);
    ko r = nodes_t::check(i.first, a, channel);
    if (is_ko(r)) {
        return r;
    }
    log("adding to nodes", i.first, i.second.first, i.second.second, peer_t::endpoint(i.second));
    nodes.emplace(i.first, a);
    return ok;
}

void c::visit(const function<void(peer_t&)>& visitor) {
    lock_guard<mutex> lock(peers.mx);
    for (auto& i: peers) {
        visitor(static_cast<peer_t&>(*i.second));
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "max edges: " <<  (*clique.begin())->size() << '\n';
}

void c::dump_random_nodes(size_t num, ostream& os) const {
    static mt19937_64 rng{random_device{}()};
    lock_guard<mutex> lock(mx_nodes);
    if (nodes.empty()) {
        os << "empty";
        return;
    }
    uniform_int_distribution<> d(0, nodes.size() - 1);
    for (int j = 0; j < num; ++j) {
        auto i = std::next(begin(nodes), d(rng));
        os << peer_t::endpoint(i->second.net_address, i->second.port) << '\n';
    }
}

void c::test_connectivity() {
    log("test connectivity");
    clique.test_connectivity();
}

void c::watch(ostream& os) const {
    os << "peers\n";
    clique.watch(os);
    faillog.dump(os);
    os << "devices\n";
    grid_dev.watch(os);
}

void c::faillog_t::add(const hostport_t& hostport) {
    ostringstream os;
    os << peer_t::endpoint(hostport);
    lock_guard<mutex> lock(mx);
    push_back(os.str());
    while(size() > 10) {
        pop_front();
    }
}

void c::faillog_t::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    if (!empty()) os << "last failed attempts:\n";
    for (auto& i: *this) {
        os << i << '\n';
    }
}

