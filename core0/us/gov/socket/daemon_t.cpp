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

#include "outbound/caller_daemon_t.h"
#include "types.h"
#include "daemon_t.h"

#define loglevel "gov/socket"
#define logclass "daemon_t"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::daemon_t;

c::daemon_t(channel_t channel, port_t port, uint8_t workers): b(channel), peers(*this), handler(*this, port, workers) {
}

c::~daemon_t() {
}

/*
client* c::create_client(sock_t sock) {
    log("KO 78673 specialize peer");
    assert(false);
    return nullptr;
//    return new peer_t(*this, sock);
}
*/

bool c::process_unhandled(client& peer, datagram* d) {
    log("process_unhandled", d->service);
    if (caller::process_unhandled(peer, d)) {
        return true;
    }
    return handler::process_unhandled(peer, d);
}

void c::detach(client& peer) {
    log("detach", peer.endpoint());
    peer.set_finished(); //follows client::on_destroy_ called from server thread - update clients - before going to gc
    if (peer.sock != -1) {
        log("shutdown recv channel");
        ::shutdown(peer.sock, SHUT_RD);
    }
    handler::task_wakeup();
}

void c::attach(client* peer) {
    log("attach", peer->sock, peer->endpoint());
    #if CFG_CONNECTION_LOG == 1
        log("writting to connlog", peer->endpoint());
        conlog << peer->endpoint() << '\n';
    #endif
    log("RBF -DISABLED BAN- lookup ban list");
    /*
    if (ban.find(peer->address) != ban.end()) {
        log("cnnected+disconnected (ban)", peer->endpoint());
        #if CFG_COUNTERS == 1
            ++counters.closed_banned;
        #endif
        delete peer; //TODO: RESTORE
        return;
    }
    */
    //churn_ctl.connected(*cl); //DDOS vector. dynamics
    log("connected", peer->endpoint());
    assert(peer->sock > 0);
    log("setting not_blocking_mode", peer->sock);
    auto r = set_not_blocking_mode(peer->sock);
    assert(r == 0);
    peers.add(peer);
    handler::task_wakeup();
}

us::ko c::start() {
    #if CFG_LOGS == 1
        assert(!logdir.empty());
        handler::logdir = logdir + "/inbound";
        caller::logdir = logdir + "/outbound";
        peers.gc.logdir = logdir + "/peers";
    #endif
    {
        auto r = caller::start();
        if (is_ko(r)) {
            log("stopped thread: recv");
            return r;
        }
        log("started thread: send");
    }
    {
        log("start");
        auto r = handler::start();
        if (is_ko(r)) {
            caller::stop();
            return r;
        }
        log("started thread: recv");
    }
    {
        auto r = peers.start();
        if (is_ko(r)) {
            handler::stop();
            caller::stop();
            return r;
        }
    }
    return ok;
}

ko c::wait_ready(const time_point& deadline) const {
    {
        auto r = handler::wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = caller::wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    return peers.wait_ready(deadline);
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::stop() {
    log("stopping thread: recv");
    handler::stop();
    log("stopping thread: send");
    caller::stop();
    log("stopping peers");
    peers.stop();
}

void c::join() {
    log("waiting for thread: recv");
    handler::join();
    log("waiting for thread: send");
    caller::join();
    log("waiting for peers");
    peers.join();
    log("stopped");
}

void c::set_busy_handlers(busyled_t::handler_t* hsend, busyled_t::handler_t* hrecv) {
    log("set_busy_handlers");
    handler::set_busy_handler(hrecv);
    caller::set_busy_handler(hsend);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "socket::daemon_t:" << '\n';
    os << prefix << "peers: \n";
    peers.dump(prefix + "  ", os);
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
}

void c::new_peer(int fd) {
    log("new_peer", fd);
    auto peer = create_client(fd);
    assert(peer != nullptr);
    log("new inbound connection", fd, peer->endpoint());
    attach(peer);
}

int c::set_not_blocking_mode(sock_t sock) {
    int ret = -1;
    int flags = ::fcntl(sock, F_GETFL, 0);
    if ((flags & O_NONBLOCK)) {
        return 0;
    }
    return ::fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

