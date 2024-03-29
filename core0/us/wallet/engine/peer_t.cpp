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
#include "peer_t.h"

#include <us/gov/relay/daemon_t.h>
#include <us/gov/relay/protocol.h>
#include <us/gov/traders/wallet_address.h>
#include <us/gov/socket/multipeer/handler_daemon_t.h>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>

#include "users_t.h"
#include "daemon_t.h"

#define loglevel "wallet/engine"
#define logclass "peer_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::peer_t;

string c::lang_en{"en"};

c::peer_t(daemon_t& daemon, sock_t sock): b(daemon, sock),
        post_auth(
            [&](pport_t rpport) {
                log("post-auth", "announcing peer in correspondence", endpoint(), "reachable at port", rpport);
                announce(rpport);
            }),
        shutdown(
            [&]() {
                log("shutdown");
            }) {
    log("peer_t constructor");
}

c::~peer_t() {
    log("peer_t destructor");
}

void c::disconnect_hilarious(datagram* d) {
    log("disconnect_hilarious", d->service);
    static constexpr array<string_view, 15> hilarious_msgs{"Rogue panda on rampage.", "Shape shifting lizard.", "Raptors ahead, caution.", "Trapped in sign factory.", "Fuck it park wherever", "All you fuckers r gonna be late", "Slow the fuck down.", "Zombies ahead.", "Zombie attack!! Evacuate.", "Smoke weed everyday.", "Poop.", "Vagina monsters ahead.", "Holyweed.", "Hack if you can.", "Sorry, your password must contain an uppercase letter, a number, a haiku, a gang sign, a hieroglyph and the blood of a virgin."};
    auto i = hilarious_msgs.begin();
    advance(i, rand() % hilarious_msgs.size());
    disconnect(d->decode_sequence(), string(*i));
    delete d;
}

bool c::process_work(datagram* d) {
    log("process_work", d->service);
    using namespace protocol;
    static_assert(engine_end <= pairing_end);
    static_assert(pairing_end <= r2r_end);
    static_assert(r2r_end <= wallet_end);
    log("process_work svc", d->service);
    if (d->service < us::gov::protocol::relay_end) {
        return b::process_work(d);
    }
    assert(d->service >= engine_begin);
    if (static_cast<auth::peer_t&>(*this).stage != auth::peer_t::authorized) {
        auto r = "KO 51150 Not authorized. Ignoring svc.";
        log(r);
        delete d;
        return true; //don't report back, disconnection happens elsewhere.
    }
    if (is_role_peer()) { //work from other remote wallet
        bool go{false};
        switch (d->service) {
            case protocol::r2r_trading_msg2:
            case protocol::r2r_trading_msg:
                go = true;
                break;
        }
        if (!go) {
            log("remote wallet sent non-trading message", d->service);
            disconnect_hilarious(d); //peer trying to access private wallet functions.
            return true;
        }
        //SECURITY: 'malicious' intentions can still reach this point, but only to fuck with a trader via trading_msgs.
    }
    if (d->service < engine_end) {
        return process_work__engine(d);
    }
    if (d->service < pairing_end) {
        return process_work__pairing(d);
    }
    if (d->service < r2r_end) {
        return process_work__r2r(d);
    }
    if (d->service < wallet_end) {
        return process_work__wallet(d);
    }
    return false;
}

ko c::authorize(const pub_t& p, pin_t pin, request_data_t& request_data) { // request_data = requested subhome
    log("authorize", p, pin);
    auto& demon = static_cast<daemon_t&>(daemon);
    if (is_role_peer() || is_role_sysop()) {
        log("role peer or sysop", "authorized", is_role_peer(), is_role_sysop());
        //string subhome_trading = ""; //the wallet I want to use to handle trades. Empty = root non-custodial wallet
        //device__w = demon.users.get_wallet(subhome_trading);
        //dev_local_w = nullptr; //we need trading handshake before knowing our local wallet
        return ok;
    }
    if (!is_role_device()) {
        auto r = "KO 40938 Unknown role";
        log(r, "not authorized");
        return r;
    }
    log("role device"); // HMI device, using wallet given by authenticator. 
    auto r = demon.authorize_device(p, pin, request_data);
    if (is_ko(r)) {
        return r;
    }
    if (!request_data.empty() && demon.devices.is_enabled__authorize_and_create_guest_wallet()) {
        ostringstream file; //revocation is writen in peer_t::handle_unpair_device (peer_t__pairing.cpp)
        file << demon.wallet_home(request_data) << "/revoked";
        if (us::gov::io::cfg0::file_exists(file.str())) {
            auto r = "KO 55710 This device has been revoked from accessing its assigned guest wallet.";
            log(r);
            return r;
        }
    }
    local_w = demon.users.get_wallet(request_data);
    log("device authorized. subhome", request_data, "local_w", local_w);
    return ok;
}

void c::announce(pport_t rpport) const {
    log("announce", rpport);
    static_cast<daemon_t&>(daemon).on_peer_wallet(pubkey.hash(), hostport.first, rpport);
}

ko c::verification_completed(pport_t rpport, pin_t pin, request_data_t& request_data) {
    log("verification_completed", endpoint(), sock, rpport, pin);
    {
        auto r = b::verification_completed(rpport, pin, request_data);
        if (is_ko(r)) {
            return r;
        }
    }
    auto ap = static_cast<us::gov::auth::peer_t*>(this);
    if (ap->stage != us::gov::auth::peer_t::authorized) {
        auto r = "KO 32032 Not authorized.";
        log(r, "disconnect-not authorized", client::endpoint());
        disconnect(0, r);
        return r;
    }
    if (is_role_peer() || is_role_sysop()) {
        log("role_peer or role_sysop");
        if (unlikely(us::gov::auth::peer_t::stage != us::gov::auth::peer_t::authorized)) {
            auto r = "KO 12011 Not authorized";
            log(r, "not_authorized->disconnect");
            disconnect(0, r);
            return r;
        }
        if (is_role_peer()) {
            log("role_peer");
            if (!static_cast<daemon_t&>(daemon).clique.add(*this, false)) {
                auto r = "KO 90547 All lines are busy.";
                log(r, "disconnect-grid doesn't accept", endpoint(), "sock", sock);
                disconnect(0, r);
                return r;
            }
            log("post_auth", "TRACE 8c");
            post_auth(rpport);
        }
        return ok;
    }
    if (is_role_device()) {
        log("role_device");
        if (!static_cast<daemon_t&>(daemon).grid_dev.add(*this, false)) {
            auto r = "KO 12001 All lines are busy.";
            log(r, "disconnect-grid_dev doesn't accept", endpoint(), sock);
            disconnect(0, r);
            return r;
        }
        return ok;
    }
    auto r = "KO 43003 Unknown role.";
    log(r, "disconnect-unknown role", client::endpoint(), sock);
    disconnect(0, r);
    return r;
}

/*
void c::schedule_push(socket::datagram* d) {
    return static_cast<daemon_t&>(daemon).pm.schedule_push(d, local_w->device_filter);
}

ko c::push_KO(ko msg) {
    return static_cast<daemon_t&>(daemon).pm.push_KO(msg, local_w->device_filter);
}

ko c::push_KO(const hash_t& tid, ko msg) {
    return static_cast<daemon_t&>(daemon).pm.push_KO(tid, msg, local_w->device_filter);
}

ko c::push_OK(const string& msg) {
    return static_cast<daemon_t&>(daemon).pm.push_OK(msg, local_w->device_filter);
}

ko c::push_OK(const hash_t& tid, const string& msg) {
    return static_cast<daemon_t&>(daemon).pm.push_OK(tid, msg, local_w->device_filter);
}
*/

svc_t c::translate_svc(svc_t svc0, bool inbound) const {
    svc_t svc;
    if (inbound) {
        svc = daemon_t::svcfish.from_prev(svc);
        log("Using API versioning translator. oldsvc ", svc0, "-> newsvc", svc);
    }
    else {
        svc = daemon_t::svcfish.to_prev(svc);
        log("Using API versioning translator. newsvc ", svc0, "-> oldsvc", svc);
    }
    return svc;
}

void c::upgrade_software() {
    log("Peer is signaling the existence of a software upgrade.");
    return static_cast<daemon_t&>(daemon).upgrade_software();
}

