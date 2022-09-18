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
#pragma once
#include <chrono>
#include <utility>
#include <us/gov/config.h>
#include <us/gov/cli/rpc_daemon_t.h>
#include <us/gov/relay/pushman.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/trader/traders_t.h>

#include "users_t.h"
#include "devices_t.h"
#include "types.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {

    struct peer_t;
    struct daemon_t;

    struct mezzanine: bgtask {
        using b = bgtask;
        mezzanine(daemon_t*);
        daemon_t* d;
    };

    /// Main algorithm of the wallet daemon, being a relay daemon (access to remote wallets and devices) and a housekeeping thread (listed in the public wallet directory))
    struct daemon_t final: us::gov::relay::daemon_t, us::wallet::engine::mezzanine {
        using b = us::gov::relay::daemon_t;
        using housekeeping = us::wallet::engine::mezzanine;
        using dispatcher_t = us::gov::datagram::dispatcher_t;
        using pushman = us::gov::relay::pushman;
        using track_status_t = us::gov::engine::daemon_t::ev_track_t::status_t;

        static const char* KO_20193, *KO_20197;

        daemon_t(channel_t, const keys_t&, port_t, pport_t, const string& home, const shostport_t& gov, uint8_t max_trade_edges, uint8_t max_devices, uint8_t workers, const string& downloads_dir);
        virtual ~daemon_t() override;

        socket::client* create_client(sock_t sock) override;
        const keys_t& get_keys() const override { return id; }
        std::pair<ko, string> authorize_deviceX(const pub_t&, pin_t pin);
        bool send_response(socket::peer_t*, datagram*, const string& payload);
        bool send_response(socket::peer_t*, datagram*, const vector<uint8_t>& payload);
        bool send_error_response(socket::peer_t*, datagram*, const string& error);
        void on_destroy_(socket::client&) override;
        ko start();
        ko wait_ready(const time_point& deadline) const;
        ko wait_ready(int seconds_deadline) const;
        void stop();
        void join();
        bool is_active() const;
        inline bool isup() const { return b::isup(); }
        pport_t get_pport() const { return b::pport; }
        void on_peer_wallet(const hash_t& addr, host_t address, pport_t rpport);
        ko announce_wallet();
        ko register_w_(host_t address);
        void wait(const chrono::seconds&);
        void run();
        void onwakeup();
        ko wait_auth();
        void configure_gov_rpc_daemon();
        string wallet_home(const string& subhome) const;
        bool has_home(const string& subhome) const;
        void on_tx_tracking_status(const track_status_t&);

        ko lookup_wallet(const hash_t& addr, hostport_t&);
        #ifdef CFG_TOPOLOGY_RING
            inline string homedir() const override { return home; }
        #endif

        string rewrite(ko) const override;

    public:
        using gov_rpc_daemon_t = gov::cli::rpc_daemon_t;
        using gov_dto = gov_rpc_daemon_t::peer_type;

        string home;
        users_t users;
        gov_rpc_daemon_t gov_rpc_daemon;
        pushman pm;
        devices_t devices;
        trader::traders_t traders;

    public:
        keys id;
        string downloads_dir;
        condition_variable cv;
        mutex mx;
        hash_t trusted_address; /// automatic updates
    };

}

