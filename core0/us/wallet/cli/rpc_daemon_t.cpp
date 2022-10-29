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
#include "rpc_daemon_t.h"

#include "hmi.h"
#include "types.h"

#define loglevel "wallet/cli"
#define logclass "rpc_daemon_t"
#include <us/gov/logs.inc>

using namespace us::wallet::cli;
using c = us::wallet::cli::rpc_daemon_t;

c::rpc_daemon_t(channel_t channel, const keys_t& keys, const shostport_t& shostport, role_t role, dispatcher_t* dispatcher): b(channel, dispatcher), id(keys), shostport(shostport), role(role), parent(nullptr) {
    log("constructor");
}

c::rpc_daemon_t(hmi& parent, const keys_t& keys, const shostport_t& shostport, role_t role, dispatcher_t* dispatcher): b(parent.p.channel, dispatcher), id(keys), shostport(shostport), role(role), parent(&parent) {
    log("constructor-hmi");
}

socket::client* c::create_client() {
    log("create_client");
    return new rpc_peer_t(*this);
}

ko c::connect() { //callback. called from the depths
    log("connect");
    return connect(parent->p.pin);
}

ko c::connect(pin_t pin) {
    log("connect with pin ", pin);
    auto r = get_peer().connect(shostport, 0, pin, role, true);
    if (is_ko(r)) {
        return r;
    }
    peer->disable_recv_timeout();
    cv_connected.notify_all();
    return ok;
}

void c::on_connect(socket::client& peer, ko r) {
    log("on_connect", (is_ko(r) ? r : "ok"));
    b::on_connect(peer, r);
    if (parent) parent->on_connect(r);
}

void c::verification_completed(bool verif_ok) {
    log("verification_completed ", verif_ok);
    if (parent) parent->verification_completed(verif_ok);
}

void c::on_I_disconnected(const string& reason) {
    log("on_I_disconnected. Reason: ", reason);
    if (parent) parent->on_I_disconnected(reason);
}

void c::on_peer_disconnected(const string& reason) {
    log("peer disconnected. Reason: ", reason);
    if (parent) parent->on_peer_disconnected(reason);
}

void c::apihelp(const string& prefix, ostream& os) {
}

void c::on_stop() {
    log("on_stop");
    b::on_stop();
    if (parent) parent->on_stop();
}

void c::upgrade_software() {
    log("Peer is signaling the existence of a upgrade_software.");
    if (parent) parent->upgrade_software();
}

