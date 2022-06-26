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
#include "net_daemon_t.h"

#include <us/gov/socket/client.h>
#include <us/gov/dfs/protocol.h>
#include <us/gov/engine/protocol.h>

#include "daemon_t.h"
#include "peer_t.h"
#include "protocol.h"
#include "local_deltas_t.h"
#include "evidence.h"

#define loglevel "gov/engine"
#define logclass "net_daemon_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::net_daemon_t;

c::net_daemon_t(engine::daemon_t& daemon, channel_t channel): b(channel), engine_daemon(daemon) {
}

c::net_daemon_t(engine::daemon_t& daemon, channel_t channel, port_t port, pport_t pport, uint8_t edges, uint8_t devices, int workers, const vector<hostport_t>& sn): b(channel, port, pport, edges, devices, workers, sn), engine_daemon(daemon) {
}

c::~net_daemon_t() {
}

const keys_t& c::get_keys() const {
    log("get_keys()");
    return engine_daemon.id;
}

string c::homedir() const {
    return engine_daemon.home;
}

const function<bool(const us::gov::socket::client&)>& c::node_filter() {
    static function<bool(const socket::client&)> f = [&](const socket::client& c)->bool { return static_cast<const peer_t&>(c).stage == peer_t::node; };
    return f;
}

const function<bool(const us::gov::socket::client&)>& c::non_sysop_filter() {
    static function<bool(const socket::client&)> f = [&](const socket::client& c)->bool { return static_cast<const peer_t&>(c).stage != peer_t::sysop; };
    return f;
}

const function<bool(const us::gov::socket::client&)>& c::sysop_filter() const {
    static function<bool(const socket::client&)> f = [&](const socket::client& c)->bool { return static_cast<const peer_t&>(c).stage == peer_t::sysop; };
    return f;
}

void c::send(const local_deltas_t& g) {
    log("brodcast local_deltas grid[0]");
    clique_send(0, 0, g.get_datagram(channel, 0));
}

void c::relay_local_deltas(datagram* d, peer_t* exclude) {
    log("relay local_deltas grid[0]");
    int n = clique_send(0, exclude, d);
    log("end relay local_deltas", n);
}

int c::relay_vote(datagram* d, peer_t* exclude) {
    int n = clique_send(0, exclude, d);
    log("relay vote", n);
    return n;
}

void c::send_vote(datagram* d) {
    log("broadcast vote. svc", d->service);
    clique_send(0, nullptr, d);
}

socket::peer_t* c::create_client(int sock) {
    log("create_client");
    return new peer_t(*this, sock);
}

bool c::check_dfs_permission(const hash_t& addr) const {
/*  RBF problem nodes joining the network. drawback: leak of ledger data to non-participants

    #ifdef CFG_PERMISSIONED_NETWORK
           lock_guard<mutex> lock(parent->auth_app->db.mx_nodes);
           auto i=parent->auth_app->db.nodes.find(addr);
           return i!=parent->auth_app->db.nodes.end();
    #endif
*/
    return true;
}

bool c::check_relay_permission(const hash_t& addr) const {
/*  RBF problem nodes joining the network
    #ifdef CFG_PERMISSIONED_NETWORK
           lock_guard<mutex> lock(parent->auth_app->db.mx_nodes);
           auto i=parent->auth_app->db.nodes.find(addr);
           return i!=parent->auth_app->db.nodes.end();
    #endif
*/
    return true;
}

//#define LOG_RELAY_EVIDENCES
#ifdef LOG_RELAY_EVIDENCES
    namespace {
        static mutex relay_ev_file_mx;
    }
#endif

void c::relay_evidence(datagram *d, peer_t* exclude) {
    assert(d->service == us::gov::protocol::engine_ev || d->service == us::gov::protocol::engine_ev_track);
    d->reset_service(us::gov::protocol::engine_ev);
    assert(d->decode_service() == us::gov::protocol::engine_ev);
    int n = clique_send(0, exclude, *d);
    #ifdef LOG_RELAY_EVIDENCES
        lock_guard<mutex> lock(relay_ev_file_mx);
        ostringstream fnos;
        fnos << LOGDIR << /relay_evidences;
        ofstream ofs(fnos.str(), ios::app);
        uint64_t now = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
        ofs << (void*)&now << ' ' << d->get_hash() << ' ' << n << " orig";
    #endif
    log("broadcast_evidence", "orig", n, "svc", d->decode_service());
    delete d;
}

