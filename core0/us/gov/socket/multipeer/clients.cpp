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
#include <fcntl.h>

#include <us/gov/config.h>
#include <us/gov/socket/peer_t.h>
#include <us/gov/socket/daemon_t.h>
#include <us/gov/socket/types.h>

#include "clients.h"

#define loglevel "gov/socket/multipeer"
#define logclass "clients"
#include <us/gov/socket/logs.inc>

using namespace us::gov::socket;
using c = us::gov::socket::multipeer::clients_t;

c::clients_t(daemon_t& d): daemon(d) {
}

c::~clients_t() {
    join();
    for (auto i: *this) delete i.second;
    cleanup();
}

ko c::init(port_t self_port) {
    shostport_t shostport = make_pair("127.0.0.1", self_port);
    return connect0(shostport);
}

ko c::init_sock2(const hostport_t& hostport) {
    log("init_sock");
    locli = ::socket(PF_INET, SOCK_STREAM, 0);
    if (locli == -1) {
        auto r = "KO 10581 locli socket initialization error.";
        log(r, errno, strerror(errno));
        return r;
    }
    sockaddr_in skaddr;
    skaddr.sin_family = AF_INET;
    skaddr.sin_port = htons(hostport.second);
    skaddr.sin_addr.s_addr = hostport.first;
    log("locli", "::connect", client::endpoint(hostport));
    int r = ::connect(locli, (struct sockaddr*) &skaddr, sizeof(skaddr));
    log("/ ::connect", client::endpoint(hostport), r);
    if (unlikely(r < 0)) {
        log(client::KO_10100, "close socket-initsock. connect returned", r, client::endpoint(hostport));
        ::close(locli);
        locli = -1;
        return client::KO_10100;
    }
    log("locli", "init_sock", "set non-block");
    fcntl(locli, F_SETFL, fcntl(locli, F_GETFL, 0) | O_NONBLOCK);
    return ok;
}

ko c::connect0(const hostport_t& hostport) {
    if (unlikely(hostport.first == 0)) {
        log(client::KO_60541);
        return client::KO_60541;
    }
    if (unlikely(hostport.second == 0)) {
        log(client::KO_58961);
        return client::KO_58961;
    }
    ko r = init_sock2(hostport);
    if (is_ko(r)) {
        log("connect-outbound-FAILED", locli, client::endpoint(hostport));
        return r;
    }
    return ok;
}

ko c::connect0(const shostport_t& shostport) {
    return connect0(client::ip4_encode(shostport));
}

void c::cleanup() {
    ::close(locli);
}

void c::add(client* peer) {
    log("add", peer->sock, "sz", size());
    lock_guard<mutex> lock(mx);
    assert(peer);
    assert(peer->sock > 0);
    auto i = find(peer->sock);
    if (likely(i == end())) {
        emplace(peer->sock, peer);
        log("added", peer->sock, "clients_sz", size());
        return;
    }
    log("fd already inn container", peer->sock, "sz", size());
    i->second->set_finished();
    i->second->sock = -1; //forget same fd from a prev connection
    log("on_destroy", i->second);
    daemon.on_destroy_(*i->second);
    gc.add_(i->second);
    i->second = peer;
    log("socket::server::clients::add> found existing client - replaced");
}

us::ko c::start() {
    log("starting gc");
    return gc.start();
}

us::ko c::wait_ready(const time_point& deadline) const {
    log("wait_ready gc");
    return gc.wait_ready(deadline);
}

void c::stop() {
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: *this) {
            i.second->disconnect(0, "rpc_api service stopped");
        }
    }
    log("stopping gc");
    gc.stop();
}

void c::join() {
    log("waiting for gc");
    gc.join();
}

void c::feedlo() {
    log("feeding loopback interface", locli);
    static char w = 'w';
    ::write(locli, &w, 1);
}

void c::update(vector<sock_t>& honest, vector<pair<host_t, string>>& evil_addr) {
    log("update", honest.size(), evil_addr.size(), "clients sz", size());
    honest.clear();
    evil_addr.clear();
    vector<client*> disconnect;
    {
        unique_lock<mutex> lock(mx);
        for (auto i = begin(); i != end(); ) {
            assert(i->second != nullptr);
            auto& c = *i->second;
            log(c.endpoint());
            if (unlikely(c.is_finished())) {
                log("is finished. reason:", get<2>(c.finished_reason));
                log("on_destroy", i->second);
                daemon.on_destroy_(*i->second);
                gc.add_(i->second);
                i = erase(i);
                log("clients_sz", size());
            }
            else {
                auto sl = c.security_level();
                log("update", "check sec lvl", sl.first);
                if (likely(sl.first == client::security_level_ok)) {
                    if (c.busyrecv.load() == 0) {
                        log("ready to receive.", c.sock, c.endpoint());
                        honest.emplace_back(c.sock);
                    }
                    else { //--strip
                        log("not ready to receive.", c.sock, c.endpoint()); //--strip
                    } //--strip
                }
                else if (sl.first == client::security_level_disconnect) {
                    log("security_level_disconnect ", c.endpoint(), "reason", sl.second);
                    c.set_finishing_reason(daemon.channel, 0, sl.second);
                    disconnect.emplace_back(i->second);
                }
                else if (sl.first == client::security_level_ban) {
                    log("security_level_ban", c.endpoint(), "reason", sl.second);
                    disconnect.emplace_back(i->second);
                }
                ++i;
            }
        }
    }
    for (auto& i: disconnect) {
        log("seclvl_disconnect ", i->endpoint());
        i->disconnect(0, "");
    }
}

void c::dump(const string& prefix, ostream& os) const {
    int n = 0;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: *this) {
            os << prefix <<"client #" << n++ << ". mem: " << i.second << '\n';
            i.second->dump_all("  ", os);
            os << '\n';
        }
    }
    os << prefix << "total active: " << n << '\n';
}

