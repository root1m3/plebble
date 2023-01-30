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
#include <us/gov/config.h>
#include <us/gov/io/seriable.h>

#include <us/wallet/trader/bootstrap/initiator_t.h>

#include "trader_t.h"
#include "bookmarks_t.h"
//#include "business.h"
#include "protocol_selection_t.h"
#include "bootstrap/protocols_t.h"
#include "personality/personality_t.h"
#include "cert/cert_t.h"
#include "cert/cert_index_t.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::engine {
    struct daemon_t;
}

namespace us::wallet::trader {

    struct business_t;
    struct trades_t;
    struct trader_t;

    struct traders_t final: unordered_map<hash_t, trader_t*>, us::gov::io::seriable {
        using hash_t = gov::crypto::ripemd160::value_type;
        using trade_id_t = hash_t;
        using b = unordered_map<trade_id_t, trader_t*>;
        using keys = us::gov::crypto::ec::keys;
        using personality_t = personality::personality_t;
        using protocols_t = bootstrap::protocols_t;
        using peer_t = engine::peer_t;

        static const char* KO_39010;

        enum service_t: uint16_t { //communications node-node
            svc_begin = 0,
            svc_ko = svc_begin,
            svc_kill_trade,

            svc_end
        };

    public:
        traders_t(wallet::local_api&);
        traders_t(const traders_t&) = delete;
        ~traders_t();

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;
        string sername() const;
        void save_state() const;
        void load_state();

    protected:
        void save_state_() const;
        void load_state_();

    public:
        using inverted_qr_t = us::wallet::trader::bootstrap::initiator_t::inverted_qr_t;

        pair<ko, hash_t> initiate(const hash_t parent_tid, const string& datadir, inverted_qr_t&&);
        void process_svc_ko(const hash_t& tid, const string& kostring);
        void on_destroy(peer_t&);
        trader_t* unlocked_trader_(const hash_t& tid);
        void erase_trader_(const hash_t& tid);
        trader_t* lock_trader_(const hash_t& tid);
        ko follow_a(peer_t&, const hash_t& tid, const string& handshake);
        ko follow_b(peer_t&, const hash_t& tid, const string& handshake);
        void list_trades(ostream&) const;
        void local_functions(ostream&) const;
        void kill(const hash_t& tid, const string& source);
        void dispose(trader_t*);
        void setup_AI();
        void on_finish();
        void dump(ostream&) const;
        void dump(const string& prefix, ostream&) const;
        void load_lf();
        //static size_t get_utid(const hash_t& tid, const us::wallet::wallet::local_api&);
        //static size_t get_utid_rootwallet(const hash_t& tid);

        void exec_help(const string& prefix, ostream&) const;
        static void help(const string& ind, ostream&);
        ko exec_shell(const string& cmd, ostream&) const; /// executes the given cmd in a sysyem shell collecting the output
        static void list_files_dir(const string& prefix, const string& dir, ostream&);
        //int set_personality(const string& sk, const string& moniker);
        //ko load_set_personality(const string& file);
        //pair<string, string> load_personality(const string& file) const;
        void reload_file(const string& fqn);

    public:
        struct cert_authority_t {
            using cert_t = us::wallet::trader::cert::cert_t;
            using cert_index_t = us::wallet::trader::cert::cert_index_t;

            static const char* KO_77965;

        public:
            cert_authority_t();
            ko init(const string& home);

        public:
            ko reset_personality(const string& sk, const string& moniker);
        
        public:
            ko cert_create(string&& msg, cert_t::options&&, hash_t& nft);
            ko cert_import(cert_t&&, hash_t& nft);
            ko cert_list(uint8_t&& id, cert_index_t&);
            ko cert_get(const hash_t& nft, cert_t&);
            ko cert_show(const hash_t& nft, string&);

            bool use_in_new_trades{false};

        private:
            ko store_(const cert_t&);
            string home;
            mutex mx;

        public:
            personality_t personality;
        };

    public: /// user < -- > traders. Trusted communication. Trusted peer. Secure. Encrypted communication
        /// Functions invoked by authorized users via API.
        ko exec(const hash_t& tid, const string& cmd); /// API command_trade
        //ko exec(const string& cmd); ///API
        ko exec(istream&);

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

    public:
        wallet::local_api& parent;
        string home;

        //using funs_t = trader_t::funs_t;
        //funs_t lf; /// local functions

        cert_authority_t ca;

        #if CFG_LOGS == 1
            string logdir;
            string trades_logdir;
        #endif

    private:
        mutable mutex _mx;

    };

}

