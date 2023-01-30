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
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "rpc_daemon_t.h"

#define loglevel "gov/socket"
#define logclass "rpc_daemon_t"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::rpc_daemon_t;

c::rpc_daemon_t(channel_t channel, dispatcher_t* dispatcher): b(channel), handler(*this, dispatcher) {
}

c::~rpc_daemon_t() {
    join();
}

bool c::sendq_active() const {
    return caller::is_active();
}

ko c::send1(client& peer, datagram* d) {
    log("send1", d);
    ko r = wait_recv_connection(true);
    if (is_ko(r)) {
        delete d;
        return r;
    }
    return caller::send1(peer, d);
}

ko c::wait_recv_connection(bool initiate) {
    return handler::wait_connection(initiate);
}

/*
client* c::create_client() {
    log("KO 68759 This class needs to be specialized.");
    assert(false);
    return nullptr;
    //return new peer_t(*this);
}
*/

bool c::process_unhandled(client& peer, datagram* d) {
    log("process_unhandled", d->service);
    if (caller::process_unhandled(peer, d)) {
        return true;
    }
    return handler::process_unhandled(peer, d);
}

void c::set_busy_handlers(busyled_t::handler_t* hsend, busyled_t::handler_t* hrecv) {
    log("set_busy_handlers");
    handler::set_busy_handler(hrecv);
    caller::set_busy_handler(hsend);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "socket::rpc_daemon_t:" << '\n';
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
}

string c::rewrite(ko r) const {
    if (r == rendezvous_t::KO_20190) {
        return peer->lasterror;  
    }
    return caller::rewrite(r);
}

us::ko c::start() {
    #if CFG_LOGS == 1
        handler::logdir = logdir + "/inbound";
        caller::logdir = logdir + "/outbound";
    #endif
    log("start");
    assert(peer == nullptr);
    auto o = create_client();
    attach(o);
    {
        auto r = caller::start();
        if (is_ko(r)) {
            return r;
        }
        log("started thread: send");
    }
    {
        auto r = handler::start();
        if (is_ko(r)) {
            caller::stop();
            log("stopped thread: send");
            return r;
        }
        log("started thread: recv");
    }
    return ok;
}

ko c::wait_ready(const time_point& deadline) const {
    ko r = handler::wait_ready(deadline);
    if (is_ko(r)) {
        return r;
    }
    return caller::wait_ready(deadline);
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::stop() {
    log("stop");
    if (peer != nullptr) {
        if (!peer->is_finished()) {
            peer->disconnect(0, "rpc_api service stopped");
            //return;
        }
    }
    log("stopping recv");
    handler::stop();
    log("stopping send");
    caller::stop();
    handler::cv_connected.notify_all();
    log("flushing rendezvous");
    caller::rendezvous.flush();
}

void c::join() {
    log("waiting for thread: recv");
    handler::join();
    log("waiting for thread: send");
    caller::join();
    delete peer;
    peer = nullptr;
}

void c::on_stop() { //if one stops stop the other
    if (caller::is_active()) caller::stop();
    if (handler::is_active()) handler::stop();
}

void c::attach(client* peer_) {
    log("attach", peer_->sock, peer_->endpoint());
    assert(peer == nullptr);
    assert(peer_->sock != 0);
    peer = static_cast<peer_t*>(peer_);
}

void c::detach(client& peer_) {
    log("detach", peer_.endpoint());
    assert(&peer_ == peer);
    peer->set_finish(); //follows client::on_destroy_ called from server thread - update clients - before going to gc
    if (peer->sock != -1) {
        log("shutdown recv channel");
        ::shutdown(peer->sock, SHUT_RD);
    }
    on_destroy_(*peer);
    if (stop_on_disconnection) {
        stop();
    }
}

