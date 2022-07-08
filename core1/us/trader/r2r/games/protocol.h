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
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>
#include <mutex>
#include <chrono>
#include <fstream>

#include <us/gov/cash/tx_t.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/track_status_t.h>

#include <us/wallet/trader/trader_protocol.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

namespace us::trader::r2r::games {

    struct business_t;

    struct protocol final: us::wallet::trader::trader_protocol {
        using b = us::wallet::trader::trader_protocol;
        using traders_t = us::wallet::trader::traders_t;
        using wallet_local_api = us::wallet::wallet::local_api;

        static constexpr const char* name{"games"};

        enum push_code_t: uint16_t { //communications node-HMI
            push_begin = trader_protocol::push_r2r_begin,
            push_tx = push_begin,

            push_end
        };

        enum service_t: uint16_t { //communications node-node
            svc_begin = trader_protocol::svc_r2r_begin,
            svc_invoice_query = svc_begin,
            svc_invoice,
            svc_cancel,
            svc_tx,

            svc_end
        };

        protocol(business_t&);
        ~protocol() override;

        ko trading_msg(peer_t&, svc_t, blob_t&&) override;
        void dump(ostream&) const override;
        void help_online(const string& indent, ostream&) const override;
        void help_onoffline(const string& indent, ostream&) const override;
        void help_show(const string& indent, ostream&) const override;
        void list_trades_bit(ostream&) const override;
        const char* rolestr() const override;
        const char* peer_rolestr() const override;
        const char* get_name() const override;
        void reset();
        blob_t push_payload(uint16_t pc) const override;
        bool requires_online(const string& cmd) const override;
        ko exec_offline(const string& cmd, ch_t&) override;
        ko exec_online(peer_t&, const string& cmd, ch_t&) override;
        static void exec_help(const string& prefix, ostream&);
        static ko exec(istream&, traders_t&, wallet_local_api&);
        uint32_t trade_state_() const;
        //void judge(const string& lang) override;

        mutable mutex mx;
    };

}

