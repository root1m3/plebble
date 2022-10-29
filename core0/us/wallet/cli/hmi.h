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
#include <us/gov/io/screen.h>
#include <us/gov/config.h>
#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg1.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/busyled_t.h>
#include <us/gov/cli/hmi0.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/chat_t.h>
#include <us/wallet/trader/bookmarks_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/data_t.h>
#include <us/wallet/trader/roles_t.h>
#include <us/wallet/trader/trader_protocol.h>

#include "params.h"
#include "fcgi.h"
#include "rpc_daemon_t.h"

namespace us::wallet::engine {
    struct daemon_t;
}

namespace us::wallet::cli {

    struct hmi: us::cli::hmi {
        using shell_args = us::gov::io::shell_args;
        using screen = us::gov::io::screen;
        using hasher_t = us::gov::crypto::ripemd160;
        using hash_t = hasher_t::value_type;
        using pub_t = us::gov::crypto::ec::keys::pub_t;
        using datagram = us::gov::socket::datagram;
        using busyled_t = us::gov::socket::busyled_t;
        using trader_protocol = us::wallet::trader::trader_protocol;
        using chat_t = us::wallet::trader::chat_t;
        using bookmarks_t = us::wallet::trader::bookmarks_t;
        using blob_t = us::wallet::trader::blob_t;

    public:
        static const char *KO_11000, *KO_10020, *KO_10021, *KO_91810, *KO_40322;

        struct dispatcher_t: us::gov::socket::datagram::dispatcher_t {
            dispatcher_t(hmi&);
            bool dispatch(us::gov::socket::datagram* d);
            hmi& m;
        };

    public:
        hmi(ostream&);
        hmi(const params&, ostream&);
        hmi(const shell_args&, ostream&);
        hmi(int argc, char** argv, ostream&);
        ~hmi() override;

        void constructor();

    public:
        void print_id() const;
        void trade(bool interactive, shell_args& args);
        bool rpc_ok();
        ko tx(shell_args&);
        static void banner(const params&, ostream&);
        static void help_2steptx(const string& ind, ostream&);
        void stop_daemon();
        void join_daemon();
        void interactive_shell();
        ko start();
        ko start(datagram::dispatcher_t*);
        ko start(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        void stop();
        void join();
        string run(); // empty ok; otw KO
        string run_();
        ko exec(const string& cmdline);
        ko exec_online1(const string& cmd, shell_args& args);
        ko exec_online(const string& cmd, shell_args& args);
        ko exec_offline(const string& cmd, shell_args& args);
        void trade_interactive(shell_args& args);
        bool trade_interactive(shell_args& args, const hash_t& tid);
        bool exec_trade_global(const string& command, shell_args& args);
        void trade_non_interactive(shell_args& args);
        static void tx_help(const string& ind, const params& p, ostream&);
        static void help(const params& p, ostream&);
        ko check_local(ostream& os, ko& ret) const;
        bool check_rpc(ostream& os, ko& ret) const;
        virtual void setup_signals(bool on);
        void set_mute(bool);
        void new_trade(const hash_t& tid);
        void enter_trade(const hash_t& tid);
        void leave_trade(const hash_t& tid);
        void leave_trade();

        virtual us::gov::socket::datagram::dispatcher_t* create_dispatcher();
        virtual pair<ko, gov::io::cfg1*> load_cfg(const string& home, bool gen) const;
        virtual void on_stop();
        static void process_cleanup();

    public:
        virtual void verification_completed(bool verif_ok);
        virtual void on_I_disconnected(const string& reason);
        virtual void on_peer_disconnected(const string& reason);
        virtual void on_connect(ko error);
        virtual string rewrite(ko r) const;

        virtual void upgrade_software();

    public:
        ko mine_public_key(const string& pattern, ostream&); //vanity addresses
        ko digest_file(const string& file, ostream&);

    public:
        ko start_daemon(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        us::wallet::engine::daemon_t* daemon{nullptr};

    public:
        ko start_rpc_daemon(busyled_t::handler_t* send, busyled_t::handler_t* recv, datagram::dispatcher_t*);
        rpc_daemon_t* rpc_daemon{nullptr};
        rpc_peer_t* rpc_peer{nullptr};
        wallet::local_api* w{nullptr};

    public:
        string home;
        mutable screen scr;
        bool interactive{false};
        params p;
        us::gov::io::cfg1* cfg{nullptr};
        hash_t cur{0};
        string curpro;

    public:
        using data_t = us::wallet::trader::data_t;
        using roles_t = us::wallet::trader::roles_t;

        data_t data;
        roles_t roles;
        mutable mutex assets_mx;

        string datasubdir;

    public:
        #if CFG_FCGI == 1
            Fastcgipp::Manager<fcgi_t>* fcgi{nullptr};
            void run_fcgi(const params&);
        #endif

    public:
        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}

