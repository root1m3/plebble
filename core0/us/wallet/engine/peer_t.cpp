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
        if (d->service != protocol::r2r_trading_msg) {
            log("remote wallet sent non-trading message", d->service);
            disconnect_hilarious(d); //peer trying to access private wallet functions.
            return true;
        }
        //SECURITY: 'malicious' intentions can still reach this point, but only to fuck with a trader via trading_msgs.
    }
    assert(wallet_local_api != nullptr);
    bool root_wallet = wallet_local_api->subhome.empty();
    if (d->service < engine_end) {
        if (!root_wallet) {
            auto r = "KO 51151 Custodial wallet. Ignoring svc.";
            log(r);
            delete d;
            return true;
        }
        return process_work__engine(d);
    }
    if (d->service < pairing_end) {
        if (!root_wallet) {
            auto r = "KO 51152 Custodial wallet. Ignoring svc.";
            log(r);
            delete d;
            return true;
        }
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

ko c::authorize(const pub_t& p, pin_t pin) {
    log("authorize", p, pin);
    auto& demon = static_cast<daemon_t&>(daemon);
    if (is_role_peer() || is_role_sysop()) {
        log("role peer or sysop", "authorized", is_role_peer(), is_role_sysop());
        wallet_local_api = demon.users.get_wallet("");
        return ok;
    }
    if (!is_role_device()) {
        auto r = "KO 40938 Unknown role";
        log(r, "not authorized");
        return r;
    }
    log("role device");
    auto r = demon.authorize_device(p, pin);
    if (is_ko(r.first)) {
        return r.first;
    }
    if (!r.second.empty() && demon.devices.is_enabled__authorize_and_create_guest_wallet()) {
        //revocation is writen in peer_t::handle_unpair_device (peer_t__pairing.cpp)
        ostringstream file;
        file << demon.wallet_home(r.second) << "/revoked";
        if (us::gov::io::cfg0::file_exists(file.str())) {
            auto r = "KO 55710 This device has its access revoked to its automatic guest wallet.";
            log(r);
            return r;
        }
    }
    wallet_local_api = demon.users.get_wallet(r.second);
    log("device authorized. subhome", r.second, "wallet local_api", wallet_local_api);
    return ok;
}

void c::announce(pport_t rpport) const {
    log("announce", rpport);
    static_cast<daemon_t&>(daemon).on_peer_wallet(pubkey.hash(), hostport.first, rpport);
}

void c::verification_completed(pport_t rpport, pin_t pin) {
    log("verification_completed", endpoint(), sock, rpport, pin);
    b::verification_completed(rpport, pin);
    if (unlikely(!verification_is_fine())) {
        log("verification_not_fine->disconnect", client::endpoint());
        disconnect(0, "KO 40043 Verification_not_fine.");
        return;
    }
    auto ap = static_cast<us::gov::auth::peer_t*>(this);
    if (ap->stage != us::gov::auth::peer_t::authorized) {
        log("disconnect-not authorized", client::endpoint());
        disconnect(0, "KO 32032 Not authorized.");
        return;
    }
    if (is_role_peer() || is_role_sysop()) {
        log("role_peer or role_sysop");
        if (unlikely(us::gov::auth::peer_t::stage != us::gov::auth::peer_t::authorized)) {
            log("not_authorized->disconnect");
            disconnect(0, "KO 12011 Not authorized");
            return;
        }
        if (is_role_peer()) {
            log("role_peer");
            if (!static_cast<daemon_t&>(daemon).clique.add(*this, false)) {
                log ("disconnect-grid doesn't accept", endpoint(), "sock", sock);
                disconnect(0, "KO 90547 All lines are busy.");
                return;
            }
            log("post_auth", "TRACE 8c");
            post_auth(rpport);
        }
    }
    else {
        if (is_role_device()) {
            log("role_device");
            if (!static_cast<daemon_t&>(daemon).grid_dev.add(*this, false)) {
                log ("disconnect-grid_dev doesn't accept", endpoint(), sock);
                disconnect(0, "KO 12001 All lines are busy.");
                return;
            }
        }
        else {
            log("disconnect-unknown role", client::endpoint(), sock);
            disconnect(0, "KO 43003 Unknown role.");
            return;
        }
    }
}

void c::schedule_push(socket::datagram* d) {
    return static_cast<daemon_t&>(daemon).pm.schedule_push(d, wallet_local_api->device_filter);
}

ko c::push_KO(ko msg) {
    return static_cast<daemon_t&>(daemon).pm.push_KO(msg, wallet_local_api->device_filter);
}

ko c::push_KO(const hash_t& tid, ko msg) {
    return static_cast<daemon_t&>(daemon).pm.push_KO(tid, msg, wallet_local_api->device_filter);
}

ko c::push_OK(const string& msg) {
    return static_cast<daemon_t&>(daemon).pm.push_OK(msg, wallet_local_api->device_filter);
}

ko c::push_OK(const hash_t& tid, const string& msg) {
    return static_cast<daemon_t&>(daemon).pm.push_OK(tid, msg, wallet_local_api->device_filter);
}

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

