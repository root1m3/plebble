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
#include <map>
#include <utility>
#include <mutex>
#include <unordered_map>
#include <fstream>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/relay/pushman.h>
#include <us/gov/config.h>

#include "trader_t.h"
#include "bookmarks_t.h"
#include "protocol_selection_t.h"
#include "bootstrap/protocols_t.h"
#include "personality/personality_t.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {
    struct daemon_t;
}

namespace us::wallet::trader {

    struct business_t;

    struct traders_t final: unordered_map<gov::crypto::ripemd160::value_type, trader_t*> {
        using hash_t = gov::crypto::ripemd160::value_type;
        using trade_id_t = hash_t;
        using b = unordered_map<trade_id_t, trader_t*>;
        using keys = us::gov::crypto::ec::keys;
        using personality_t = personality::personality_t;
        using protocols_t = bootstrap::protocols_t;
        using peer_t = engine::peer_t;

        traders_t(engine::daemon_t&, const string& home);
        traders_t(const traders_t&) = delete;
        ~traders_t();

        enum service_t: uint16_t { //communications node-node
            svc_begin = 0,
            svc_ko = svc_begin,
            svc_kill_trade,

            svc_end
        };

        void process_svc_ko(const hash_t& tid, const string& kostring);
        void on_destroy(peer_t&);
        pair<ko, hash_t> initiate(const hash_t parent_tid, const string& datadir, qr_t&&, wallet::local_api&);
        trader_t* unlocked_trader_(const hash_t& trade_id);
        void erase_trader_(const hash_t& trade_id);
        trader_t* lock_trader_(const hash_t& trade_id);
        ko follow_a(peer_t& peer, const hash_t& trade_id, const string& handshake);
        ko follow_b(peer_t& peer, const hash_t& trade_id, const string& handshake);
        void list_trades(const hash_t& subhomeh, ostream&) const;
        void api_list_protocols(ostream&) const;
        void published_protocols(protocols_t&, bool invert) const;
        void invert(protocols_t&) const;
        protocols_t published_protocols(bool invert) const;
        void local_functions(ostream&) const;
        pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&);
        pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&&);
        pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&, params_t&& remote_params);
        void kill(const hash_t&, const string& source);
        void dispose(trader_t*);
        void setup_AI();
        void on_finish();
        void dump(ostream&) const;
        void dump(const string& prefix, ostream&) const;
        void load_plugins();
        void load_bank();
        void load_lf();
        void exec_help(const string& prefix, ostream&) const;
        static void help(const string& ind, ostream&);
        ko exec_shell(const string& cmd, ostream&) const; /// executes the given cmd in a sysyem shell collecting the output
        static void list_files_dir(const string& prefix, const string& dir, ostream&);
        int set_personality(const string& sk, const string& moniker);
        ko load_set_personality(const string& file);
        pair<string, string> load_personality(const string& file) const;
        void reload_file(const string& fqn);

    public: /// user < -- > traders. Trusted communication. Trusted peer. Secure. Encrypted communication
        /// Functions invoked by authorized users via API.
        ko exec(const hash_t& tid, const string& cmd, wallet::local_api&); /// API command_trade
        ko exec(const string& cmd, wallet::local_api&); ///API
        ko exec(istream&, wallet::local_api&);

    public: /// traders < -- > traders. Untrusted peer. Secure. Encrypted communication
        /// Functions invoked by remote instances.
        ko trading_msg(peer_t&, svc_t svc, const hash_t& tid, blob_t&& msg);
        void on_file_updated2(const string& path, const string& name, const trader_t* source_trader);

    public:
        ko start();
        ko wait_ready(const time_point& deadline) const;
        void stop();
        void stop_();
        void join();

        struct lib_t final {
            lib_t(const string& name, const string& libfilename, const string& home);
            lib_t(business_t*);
            lib_t(const lib_t&) = delete;
            ~lib_t();
            using pushman = us::gov::relay::pushman;
            void dump(ostream&) const;
            void invert(protocols_t&) const;
            void published_protocols(protocols_t&, bool invert) const;
            void exec_help(const string& prefix, ostream&) const;
            pair<protocol_selection_t, bookmark_info_t> bookmark_info() const;
            ko exec(istream&, traders_t&, wallet::local_api&);

            void* plugin{nullptr};
            business_t* business;
        };

        struct libs_t final: map<string, lib_t*> {
            libs_t();
            libs_t(const libs_t&) = delete;
            ~libs_t();
            using pushman = lib_t::pushman;
            void dump(ostream&) const;
            void invert(protocols_t&) const;
            void published_protocols(protocols_t&, bool invert) const;
            pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&);
            pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&&);
            void exec_help(const string& prefix, ostream&) const;
            ko exec(istream&, traders_t&, wallet::local_api&);
            void bookmark_info(vector<pair<protocol_selection_t, bookmark_info_t>>&) const;
        };

        struct bookmarksman_t final: bookmarks_t {
            using b = trader::bookmarks_t;

            bookmarksman_t();
            ko init(const string& home);
            ko load();
            ko save();
            ko load_();
            ko save_();
            blob_t blob() const;
            ko add(const string& name, bookmark_t&&);
            ko remove(const string& key);

            string home;
            mutable mutex mx;
        };

        void published_bookmarks(wallet::local_api&, bookmarks_t&) const;

    public:
        void schedule_push(socket::datagram*, wallet::local_api&);
        ko push_KO(ko msg, wallet::local_api&);
        ko push_KO(const hash_t& tid, ko msg, wallet::local_api&);
        ko push_OK(const string& msg, wallet::local_api&);
        ko push_OK(const hash_t& tid, const string& msg, wallet::local_api&);

    public:
        libs_t libs;

        engine::daemon_t& daemon;
        string home;

        using funs_t = trader_t::funs_t;
        funs_t lf; //local functions

        personality_t personality;
        bookmarksman_t bookmarks;
        mutable mutex bookmarks_mx;
        #if CFG_LOGS == 1
            string logdir;
            string trades_logdir;
        #endif

    private:
        mutable mutex _mx;

    };

}

