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

#include <random>
#include <iterator>
#include <string_view>
#include <array>

#include <us/gov/crypto/ec.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/client.h>
#include <us/gov/traders/wallet_address.h>
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/endpoint_t.h>
#include <us/wallet/wallet/local_api.h>

#include "peer_t.h"
#include "rpc_peer_t.h"

#define loglevel "wallet/engine"
#define logclass "daemon_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::daemon_t;

constexpr auto RC_InProgress = "Announced";
const char* c::KO_20193 = "KO 20193 Wallet not available.";
const char* c::KO_20197 = "KO 20197 Invalid datagram.";

us::wallet::engine::mezzanine::mezzanine(daemon_t* d): d(d), b(bind(&c::run, d), bind(&c::onwakeup, d)) {
}

namespace {

    struct my_dispatcher_t: us::gov::socket::datagram::dispatcher_t {
        using b = us::gov::socket::datagram::dispatcher_t;

        my_dispatcher_t(c& daemon): daemon(daemon) {
        }

        bool dispatch(datagram* d) override {
            log("gov dispatch"); 
            if (d->service != us::gov::protocol::engine_track_response) {
                return false;
            }
            using track_status_t = us::gov::engine::daemon_t::ev_track_t::status_t;
            track_status_t status;
            auto r = blob_reader_t::readD(*d, status);
            delete d;
            if (is_ko(r)) {
                log("engine_track", r);
                return true;
            }
            log("engine_track");
            daemon.on_tx_tracking_status(status);
            return true;
        }

        c& daemon;
    };

}

c::daemon_t(channel_t channel, const keys_t& k, port_t port, pport_t pport, const string& home_, const shostport_t& backend, uint8_t max_trade_edges, uint8_t max_devices, uint8_t workers, const string& downloads_dir):
        id(k),
        b(channel, port, pport, max_trade_edges, max_devices, workers),
        downloads_dir(downloads_dir),
        housekeeping(this),
        trusted_address(AUTOMATIC_UPDATES_ACCOUNT),
        home(home_),
        gov_rpc_daemon(channel, k, backend, rpc_peer_t::role_device, new my_dispatcher_t(*this)),
        pm(*this),
        users(*this),
        devices(home_),
        traders(*this, home_ + "/trader") {

    assert(!home.empty());
    log("downloads directory:", downloads_dir);
    io::cfg0::ensure_dir(downloads_dir);
}

c::~daemon_t() {
    join();
}

void c::wait(const chrono::seconds& d) {
    if (housekeeping::reset_wait()) {
        log("no-wait");
        return;
    }
    log("waiting", d.count(), "secs");
    {
        unique_lock<mutex> lock(mx);
        cv.wait_for(lock, d, [&] { return housekeeping::isdown(); });
    }
    housekeeping::reset_wait();
}

ko c::register_w_(host_t addr) {
    log ("register_w_", addr);
    blob_t blob;
    {
        us::gov::traders::wallet_address ev(id.pub.hash(), addr, pport);
        ev.sign(id);
        ev.write(blob);
    }
    return gov_rpc_daemon.get_peer().call_ev(move(blob));
}

string c::rewrite(ko r) const {
    if (r == us::gov::socket::rendezvous_t::KO_20190) { //ko reported by gov daemon
        return gov_rpc_daemon.rewrite(r);
    }
    return b::rewrite(r);
}

ko c::announce_wallet() {
    log("lookup myself ", id.pub.hash());
    {
        gov_dto::lookup_wallet_out_dst_t o_out;
        auto r = gov_rpc_daemon.get_peer().call_lookup_wallet(id.pub.hash(), o_out);
        if (is_ok(r)) {
            log("Already in wallet directory", gov::socket::client::endpoint(o_out.net_addr, o_out.port));
            return ok;
        }
    }
    gov_dto::random_wallet_out_dst_t o_out;
    {
        auto r = gov_rpc_daemon.get_peer().call_random_wallet(o_out);
        if (is_ko(r)) {
            log("Cannot find a wallet for registering.");
            return r;
        }
    }
    auto ep = make_pair(o_out.net_addr, o_out.port);
    log("Register my wallet: Connecting to random wallet", o_out.addr, us::gov::socket::client::endpoint(ep));
    /// Sequence. Function hook executed
    /// 1 offline. pre-connect -> Set a hook for post-auth
    /// 2 connected. pre-attach -> just wait a few seconds so the remote peer has time to do the announcement diligence
    /// 3 auth. post-auth -> Do the announcement diligence for the peer in correspondence.
    peer::peer_t* peer;
    auto r = grid_connect(ep,
        [](peer::peer_t* p) {
            log("pre-connect");
            static_cast<peer_t*>(p)->post_auth = [p](pport_t rpport) {
                log("post-auth", "announcing peer in correspondence", p->endpoint(), "reachable at port", rpport);
                static_cast<peer_t*>(p)->announce(rpport); //we announce peer in correspondence
            };
        },
        [&](peer::peer_t* p) {
            log("post-connect/pre-attach");
            log("Another wallet is aware of me. Soon I'll be in the ledger.", "Disconnect in 5s, next task in 120s");
            ++p->sendref;
            peer = p;
        });
    if (is_ko(r)) {
        return r;
    }
    log("disconnect buddy");
    wait(5s);
    log("disconnect buddy");
    peer->disconnect(0, "Thanks for broadcasting evidence of me.");
    --peer->sendref;
    log("Announced");
    return RC_InProgress;
}

void c::run() {
    #if CFG_LOGS == 1
        log_start(logdir, "housekeeping");
    #endif
    log("starting housekeeping thread");
    chrono::seconds t = 10s;
    while(housekeeping::isup()) {
        log("housekeeping tasks");
        {
            log("task: announce wallet:");
            auto r = announce_wallet();
            if (r == ok) {
                t = 600s; /// low freq re-check
            }
            else if (r == RC_InProgress) {
                t = 120s; /// check for completion
            }
            else {
                t = 10s; /// high freq check
            }
        }
        wait(t);
    }
    log("stopped housekeeping thread");
}

void c::onwakeup() {
    cv.notify_all();
}

void c::on_destroy_(socket::client& peer) {
    log("on_destroy");
    static_cast<peer_t&>(peer).shutdown(); ///shutdown hook
    b::on_destroy_(peer); //remove from grid, then go to gc
}

ko c::start() {
    log("starting daemon");
    #if CFG_LOGS == 1
        assert(!logdir.empty());
        gov_rpc_daemon.logdir = logdir + "/gov_rpc_daemon";
        users.logdir = logdir + "/users";
        pm.logdir = logdir;
        pm.logfile = "pushman";
        traders.logdir = logdir + "/traders";
    #endif
    log("starting server");
    {
        auto r = b::start();
        if (unlikely(is_ko(r))) {
            log(r);
            return r;
        }
    }
    log("starting gov_rpc_daemon");
    configure_gov_rpc_daemon();
    {
        auto r = gov_rpc_daemon.start();
        if (unlikely(is_ko(r))) {
            b::stop();
            return r;
        }
    }
    log("starting traders");
    {
        auto r = traders.start();
        if (unlikely(is_ko(r))) {
            log(r);
            housekeeping::stop();
            pm.stop();
            gov_rpc_daemon.stop();
            b::stop();
            return r;
        }
    }
    log("starting pushman");
    {
        auto r = pm.start();
        if (unlikely(is_ko(r))) {
            traders.stop();
            gov_rpc_daemon.stop();
            b::stop();
            return r;
        }
    }
    log("starting daemon::housekeeping");
    {
        auto r = housekeeping::start();
        if (unlikely(is_ko(r))) {
            log(r);
            pm.stop();
            traders.stop();
            gov_rpc_daemon.stop();
            b::stop();
            return r;
        }
    }
    return ok;
}

socket::client* c::create_client(sock_t sock) {
    log("create_client", sock);
    return new peer_t(*this, sock);
}

pair<ko, string> c::authorize_device(const pub_t& p, pin_t pin) {
    log("is device authorized?", pin);
    return devices.authorize(p, pin);
}

void c::on_peer_wallet(const hash_t& addr, host_t address, pport_t rpport) {
    log ("on_peer_wallet", addr, address, rpport);
    if (trader::endpoint_t::is_ip4(addr)) {
        log("KO 20199 address came as IP4 encoded. Not registering.", addr);
        return;
    }
    blob_t blob;
    {
        us::gov::traders::wallet_address ev(addr, address, rpport);
        ev.sign(id);
        ev.write(blob);
    }
    gov_rpc_daemon.get_peer().call_ev(move(blob));
}

void c::on_tx_tracking_status(const track_status_t& status) {
    log("on_tx_tracking_status", us::gov::engine::evt_status_str[status.st], status.from, status.to);
    users.on_tx_tracking_status(status);
}

/*//////////////////////////////////////
  ^ ^  ^ ^  _ ( _    .  (     _  _  _ _
 / v \/ v \   _)|\/|)|()_) * |_ |_|| | |
    This is not a photo opportunity
//////////////////////////////////////*/

bool c::is_active() const {
    return b::is_active() && gov_rpc_daemon.is_active();
}

us::ko c::wait_ready(const time_point& deadline) const {
    log("starting gov endpoint");
    {
        auto r = b::wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = gov_rpc_daemon.wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = pm.wait_ready(deadline);
        if (is_ko(r)) {
            return r;
        }
    }
    return traders.wait_ready(deadline);
}

ko c::wait_ready(int seconds_deadline) const {
    return wait_ready(chrono::system_clock::now() + chrono::seconds(seconds_deadline));
}

void c::stop() {
    log("stopping daemon::housekeeping");
    housekeeping::stop();
    log("stopping pushman");
    pm.stop();
    log("stopping traders");
    traders.stop();
    log("stopping gov_rpc_daemon");
    gov_rpc_daemon.stop();
    log("stopping daemon");
    b::stop();
}

void c::join() {
    log("waiting for daemon::housekeeping");
    housekeeping::join();
    log("waiting for pushman");
    pm.join();
    log("waiting for traders");
    traders.join();
    log("waiting for gov_rpc_daemon");
    gov_rpc_daemon.join();
    log("waiting for server");
    b::join();
}

ko c::wait_auth() {
    return gov_rpc_daemon.get_peer().wait_auth();
}

bool c::has_home(const string& subhome) const {
    if (subhome.empty()) return true;
    ostringstream os;
    os << home << "/guest/" << subhome;
    return io::cfg0::dir_exists(os.str());
}

string c::wallet_home(const string& subhome) const {
    if (subhome.empty()) return home;
    ostringstream os;
    os << home << "/guest/" << subhome;
    io::cfg0::ensure_dir(os.str());
    return os.str();
}

void c::configure_gov_rpc_daemon() {
    gov_rpc_daemon.stop_on_disconnection = false;
    gov_rpc_daemon.connect_for_recv = true;
    gov_rpc_daemon.encrypt_traffic = true;
    //bool trusted_localhost = true; // disable for systems where IPC is not safe. provably true for machines controlled by the user.
    //if (trusted_localhost) {
    if (gov_rpc_daemon.shostport.first == "localhost" || gov_rpc_daemon.shostport.first == "127.0.0.1") {
        log("trusted_localhost is set to 1: disabling encryption on IPC calls between wallet and gov as they are in the same machine. govd at", gov_rpc_daemon.shostport.first);
        gov_rpc_daemon.encrypt_traffic = false;
    }
    //}
}

ko c::lookup_wallet(const hash_t& addr, hostport_t& hostport) {
    log("lookup_wallet", addr);
    gov_rpc_daemon_t::peer_type::lookup_wallet_out_dst_t fc_out;
    auto r = gov_rpc_daemon.get_peer().call_lookup_wallet(addr, fc_out);
    if (is_ko(r)) {
        return r;
    }
    switch (fc_out.net_addr) {
        case 2130706433: //127.0.0.1
        case 0:
            r = "KO 30299 Resolved to 127.0.0.1 or 0. Resolve to localhost is disabled.";
            log(r, gov::socket::client::endpoint(fc_out.net_addr, fc_out.port));
            return r;
    }
    hostport.first = fc_out.net_addr;
    hostport.second = fc_out.port;
    return ok;
}

