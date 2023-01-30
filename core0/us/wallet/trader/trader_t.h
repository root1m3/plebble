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

#include <utility>
#include <thread>
#include <vector>
#include <atomic>
#include <tuple>
#include <map>
#include <set>
#include <fstream>
#include <sstream>

#include <us/gov/config.h>
#include <us/gov/bgtask.h>
#include <us/gov/io/seriable.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/ec.h>

#include "types.h"
#include "trader_protocol.h"
#include "chat_t.h"
#include "endpoint_t.h"
#include "protocol_selection_t.h"
#include "conman.h"
#include "kv.h"
#include "data_t.h"
#include "bootstrap/bootstrapper_t.h"
#include "bootstrap/handshake_t.h"
#include "bootstrap/dialogue_a_t.h"
#include "bootstrap/dialogue_b_t.h"
#include "bootstrap/dialogue_c_t.h"
#include "bootstrap/protocols_t.h"
#include "personality/personality_t.h"
#include "personality/proof_t.h"

namespace us::wallet::engine {
    struct peer_t;
}

namespace us::wallet::trader {
    using namespace std;
    struct traders_t;

    struct trader_t final: conman, us::gov::io::seriable {
        using b = conman;
        using protocol = trader_protocol;
        using bootstrapper_t = bootstrap::bootstrapper_t;
        using personality_t = personality::personality_t;
        using personality_proof_t = personality::proof_t;
        using protocols_t = bootstrap::protocols_t;
        using params_t = us::wallet::trader::params_t;
        using priv_t = us::gov::crypto::ec::keys::priv_t;
        using challenge_t = trader::personality::proof_t::challenge_t;
        using peerid_t = hash_t;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;
        using serid_t = blob_reader_t::serid_t;

    public:
        trader_t(traders_t& parent, engine::daemon_t& demon, const hash_t& parent_tid, const string& datadir);
        ~trader_t();

    public:
        ko permission_bootstrap(const peerid_t& peer) const;
        pair<ko, hash_t> boot(const peerid_t&, bootstrapper_t*);
        pair<ko, hash_t> boot(const hash_t& tid, wallet::local_api&);
        void init(const hash_t& tid, const endpoint_t&, wallet::local_api&);

    public:
        void on_KO(ko, const string&) override;
        void online(peer_t&) override;
        void offline() override;
        void on_stop() override;
        bool requires_online(const string& cmd) const override;
        ko exec_online(peer_t& peer, const string&, ch_t&) override;
        ko exec_offline(const string&, ch_t&) override;
        const endpoint_t& local_endpoint() const override;
        pair<ko, hostport_t> resolve_ip_address(const hash_t& address) const override;
        void die(const string& reason) final override;
        void dispose() final override;
        void saybye(peer_t&) final override;
        ko update_peer(peer_t&, ch_t&&) const override;

    public:
        ko deliver(personality_proof_t&&, ch_t&);
        ko deliver(personality_proof_t&&, params_t&&, ch_t&);
        ko deliver(params_t&&, ch_t&);
        ko deliver(protocols_t&&, challenge_t&&, ch_t&);
        ko deliver(protocol_selection_t&&, params_t&&, ch_t&);
        ko deliver(challenge_t&&, protocol_selection_t&&, params_t&&, ch_t&);
        ko deliver(endpoint_t&&, protocols_t&&, challenge_t&&, personality_proof_t&&, ch_t&);
        ko deliver(endpoint_t&&, challenge_t&&, personality_proof_t&&, params_t&&, ch_t&);
        void write_personality_proof(ostream&) const;
        void file_reloadx(const string& fqn);
        void update_activity();
        ko exec_trader_(client&, const string& cmd, istream&);
        bool resume_chat(peer_t&);
        ko trading_msg(peer_t&, svc_t svc, blob_t&&);
        ko trading_msg_trader(peer_t&, svc_t svc, blob_t&&);
        void dump(const string& prefix, ostream&) const override;
        void list_trades(ostream&) const;
        void help(const string& indent, ostream&) const;
        void flush_command_queue();
        void on_file_updated(const string& path, const string& name);
        void on_signal(int sig);
        bool sig_reset(ostream&);
        bool sig_hard_reset(ostream&);
        bool sig_reload(ostream&);

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;
        serid_t serial_id() const override { return 'T'; }
        pair<string, string> sername() const;
        void save_state() const;
        void load_state(const hash_t& tid);
        
    public:
        enum push_code_t: uint16_t { //communications node-HMI
                push_begin = 100,
            push_log = push_begin, //100
            push_data,
            push_peer_personality,
            push_roles_mine,
            push_roles_peer,
            push_roles,
            push_qr_mine,
            push_qr_peer,
            push_trade,
            push_killed,
            push_chat,
            push_bookmarks,
            push_help,
            push_cert,
            //push_local_functions,
            //push_remote_functions,

            push_end
        };

        enum service_t: uint16_t { //communications node-node
                svc_begin = 100,
            svc_ping = svc_begin, //100
            svc_pong,
                svc_handshake_begin,
            svc_handshake_a1 = svc_handshake_begin,
            svc_handshake_a2,
            svc_handshake_a3,
            svc_handshake_b1,
            svc_handshake_b2,
            svc_handshake_b3,
            svc_handshake_c1,
            svc_handshake_c2,
            svc_handshake_c3,
            svc_handshake_c4,
                svc_handshake_end,
            svc_roles_request = svc_handshake_end,
            svc_roles,
            svc_qr_request,
            svc_qr,
            svc_personality,
            svc_personality_and_params,
            svc_end_protocol,
            svc_chat_msg,
            svc_functions_request,
            svc_functions,
            svc_cert,

            svc_end
        };

    public:
        chat_t::entry AI_chat(const chat_t&, peer_t&);
        ko send_msg(peer_t&, const string&);
        ko send_msg(peer_t&, const chat_t::entry&);
        void send_msg(const string&);
        inline bool has_protocol() const { return p != nullptr; }
        void show_data_(const string& lang, ostream&) const;
        void show_data(const string& lang, ostream&) const;
        void write_data(const string& lang, blob_t&) const;
        void ping(function<void(uint64_t)> pong_handler);
        void ping(peer_t&, function<void(uint64_t)> pong_handler);
        void ping(peer_t&);
        void reset_ping();
        inline bool initiator() const { return bootstrapper == nullptr ? true : bootstrapper->initiator(); }
        int set_personality(const string& sk, const string& moniker);
        void feedback_devices(const ch_t&) const;
        ko on_remote(personality_proof_t::raw_t&&, ch_t&); //returns changes as a result of re-setting peer's personality
        ko on_remote(personality_proof_t&&, ch_t&); //returns changes as a result of re-setting peer's personality, if proof validates
        ko on_remote(remote_params_t *rp, ch_t&); //returns changes as a result of re-setting peer's shared params
        ko on_remote(personality_proof_t&&, remote_params_t *rp, ch_t&); //returns changes as a result of re-setting both peer's personality and shared params
        ko on_svc_personality_and_params(blob_reader_t&, ch_t&);
        ko on_svc_personality(blob_reader_t&, ch_t&);
        void update_peer(ch_t&&);
        protocol_selection_t opposite_protocol_selection() const;
        params_t shared_params() const;
        qr_t remote_qr() const;
        ko create_bookmark(const string& name, string&& icofile, string&& label);
        ko create_bookmark(const string& name, const bookmark_t&);
        void load_my_protocols();
        ko push_KO(ko) const;
        ko push_OK(const string& msg) const;

/*
        struct funs_t: us::gov::io::seriable_set<string> {
            funs_t() {}
            void dump(ostream&) const;
            void exec_help(const string& prefix, ostream&) const;
        };
*/

        template<typename... Args> void olog(const Args&... args) const;

    public:
        inline void schedule_push_data(const string& lang) const { schedule_push(push_data, lang); }
        void schedule_push(datagram*) const;
        void schedule_push(uint16_t code, const string& lang) const;
        blob_t push_payload(uint16_t pc, const string& lang) const;
        [[nodiscard]] datagram* get_push_datagram(uint16_t pc, const string& lang) const;

    public:
        hash_t id;
        hash_t parent_tid;
        personality_t my_personality;
        personality_proof_t::raw_t peer_personality;
        protocols_t my_protocols;
        protocols_t peer_protocols;
        bookmarks_t peer_qrs;
        ts_t ts_creation;
        challenge_t my_challenge;
        challenge_t peer_challenge{0};
        bootstrapper_t* bootstrapper{nullptr};
        peerid_t bootstrapped_by{0};
        chat_t chat;
        string datasubdir;

    private:
        hash_t endpoint_pkh;
        //funs_t rf; //remote functions

        trader_protocol* p{nullptr};

        atomic<uint64_t> ts_activity;

    public:
        //pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&&);
        pair<ko, hash_t> initiate(qr_t&&);
        //pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&, params_t&& remote_params);
        ko set_protocol(trader_protocol*, ch_t&);
        ko set_protocol_(trader_protocol*, ch_t&);

        ko call_trading_msg(peer_t& peer, const uint16_t& code, const blob_t& payload) const;

    public:
        wallet::local_api* w{nullptr};

        traders_t& parent; //following pointer could lead to priviledge scalation in custodial wallets
        mutable mutex mx;

    public:
        #if CFG_LOGS == 1
            mutable data_t prev_data;
        #endif

    private:
        int peer_mutations{0};
        function<void(uint64_t)> pong_handler = [](uint64_t){};
        ts_t ts_sent_ping{0};
        route_t route;
    };

}

