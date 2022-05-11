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
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string_view>
#include <map>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/engine/signed_data.h>
#include <us/gov/relay/pushman.h>
#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/personality/personality_t.h>

#include "params_t.h"
#include "protocol_selection_t.h"
#include "business.h"
#include "chat_t.h"
#include "ch_t.h"

namespace us::wallet::engine {

    struct peer_t;

}

namespace us::wallet::trader {

//    using namespace us::gov;

/*
    /// Resposible of:
    ///   source bitmap (advertisement),
    ///   pay per view.
    ///   Protocol goes:
    ///     Alice is willing to pay 1 AliceCoin to anyone who ask for watching it.
    ///     Bob sends a svc_req_ad with message, representing a view-the-ad-promise (regardles of the payment issued by alice). He attaches his target_profile so the advertiser can be more accurate serving an effective ad to him).
    ///     Bernard is willing to see the add but for at least 15 AliceCoins. He sends the same message attached with an invoice to be fulfilled by Alice.
    ///     Alice, who received an invoice sends back an incomplete signed tx (although complete on the side of Alice) together with the ad payload. (a
    ///     Bob Receives a partial tx where he would need to complete with its information (recp account)
    ///     Bernard Receives a partial tx. If Alice added extrta outputs good for him but at least he knows that if the payment is less of 15 AliceCoin he would discard the Ad and charge whatever the amount Alice sent anyway.
    struct advertiser_t {
        vector<uint8_t> ad;
        hash_t reward_coin;
        cash_t amount_to_pay;
    };
*/
    struct business_t;
    struct trader_t;
    struct traders_t;

    struct trader_protocol {
        using signed_data = us::gov::engine::signed_data;
        using pushman_t = us::gov::relay::pushman;
        using params_t = us::wallet::trader::params_t;
        using local_params_t = us::wallet::trader::local_params_t;
        using cash_t = us::gov::cash_t;
        using chat_t = us::wallet::trader::chat_t;
        using peer_t = us::wallet::engine::peer_t;
        using trader_t = us::wallet::trader::trader_t;
        using ch_t = trader::ch_t;
        using business_t = us::wallet::trader::business_t;
        using blob_reader_t = us::gov::io::blob_reader_t;
        using blob_writer_t = us::gov::io::blob_writer_t;

        static const char *WP_29101, *KO_29100; //WayPoint

        enum push_code_t: uint16_t { //communications node-HMI
            push_begin = 200,
            push_ico = push_begin, // push 200
            push_logo,
            push_params,
            push_tip,

            push_end,
            push_r2r_begin = 300
        };

        enum service_t: uint16_t { //communications node-node
            svc_begin = 200,
            svc_ico_request = svc_begin, //svc 200
            svc_ico,  //svc 201
            svc_logo_request, //svc 202
            svc_logo,  //svc 203
            svc_params,  //svc 204

            svc_end, //svc end
            svc_r2r_begin = 300
        };

        static void twocol(const string& prefix, const string_view& l, const string_view& r, ostream&);
        static void twocol(const string& prefix, const string_view& l, const string_view& r, const string_view& r2, ostream&);
        static void exec_help(const string& prefix, ostream&);
        static trader_protocol* from_stream(istream&);
        static ko exec(istream&, traders_t&, wallet::local_api&);

        trader_protocol(business_t&);
        virtual ~trader_protocol();

        virtual void list_trades_bit(ostream&) const = 0;
        virtual const char* get_name() const = 0;
        virtual const char* rolestr() const = 0;
        virtual const char* peer_rolestr() const = 0;
        virtual void dump(ostream&) const;
        virtual void help_online(const string& indent, ostream&) const;
        virtual void help_onoffline(const string& indent, ostream&) const;
        virtual void help_show(const string& indent, ostream&) const;
        virtual void data(const string& lang, ostream&) const;
        virtual ko trading_msg(peer_t&, svc_t svc, blob_t&&);
        virtual ko exec_online(peer_t&, const string& cmd, ch_t&);
        virtual ko exec_offline(const string& cmd, ch_t&);
        virtual bool requires_online(const string& cmd) const;
        virtual void on_finish();
        virtual ko on_attach(trader_t&, ch_t&); //local, remote params changed, personality changed
        virtual bool on_signal(int sig, ostream&);
        virtual string datadir(const local_params_t&) const;
        virtual void on_file_updated(const string& path, const string& name, ch_t&);
        virtual bool underride_default_params(local_params_t&) const;
        virtual blob_t push_payload(uint16_t pc) const;
        datagram* get_push_datagram(uint16_t pc) const;
        virtual chat_t::entry AI_chat(const chat_t&, peer_t&);
        ko send_shared_bookmarks(peer_t&) const;
        void on_file_updated(const string& path, const string& name);
        ko attach(trader_t&, ch_t&);
        template<typename... Args> void olog(const Args&... args) const;
        const hash_t& tid() const;
        wallet::local_api& w();
        const wallet::local_api& w() const;
        void schedule_push(uint16_t code) const;
        void to_stream(ostream&) const;
        protocol_selection_t opposite_protocol_selection() const;
        protocol_selection_t protocol_selection() const;
        bool set_personality_from_params(ch_t&);
        ko update_peer_(peer_t&, ch_t&&) const;
        params_t shared_params() const;
        ko on_svc_params(blob_reader_t&, ch_t&);
        ko on_svc_params_(blob_reader_t&, ch_t&);
        ko rehome_apply(ch_t&); //shared params; personality changed
        ko on_remote(const personality::proof_t::raw_t&, ch_t&);
        ko on_remote(params_t*, ch_t&);
        ko on_remote_(const personality::proof_t::raw_t&, ch_t&);
        virtual ko on_remote_(params_t*, ch_t&); //local, remote params changed
        virtual ko rehome(ch_t&); //Produces ppdir from pdir
        bool rehome_dir(const string& dir, ch_t&); /// returns if personality changed
        bool sig_reset(ostream&);
        bool sig_hard_reset(ostream&);
        void sig_reload(ostream&);
        void save_params_template();
        bool has_remote_params() const;
        string get_lang() const;

    public:
        local_params_t _local_params;
        mutable mutex _local_params_mx;
        remote_params_t* remote_params{nullptr};
        mutable mutex remote_params_mx;
        business_t& business;
        string phome; //protocol home. e.g. /home/gov/.us/wallet/trader/pat2ai/ai
        string pphome; ////custom peer home. e.g. /home/gov/.us/wallet/trader/pat2ai/ai/BCnR6X64EVMPsXEFsdf3MkdMRxV
        trader_t* tder{nullptr};
        blob_t image; //remote logo
        blob_t ico; //remote ico
        mutable mutex assets_mx;

        static ko hold_remote_params(blob_t&&);
        static remote_params_t* remote_params_on_hold;
    };

}

