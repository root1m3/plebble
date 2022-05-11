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

#include <us/wallet/trader/workflow/trader_protocol.h>
#include <us/wallet/wallet/txlog_t.h>

namespace us::wallet::trader {
    struct traders_t;
}

namespace us::wallet::trader::r2r::w2w {

    struct business_t;

    struct protocol final: us::wallet::trader::workflow::trader_protocol {
        using b = us::wallet::trader::workflow::trader_protocol;
        using txlog_t = us::wallet::wallet::txlog_t;
        using track_t = us::wallet::wallet::track_t;
        using tx_t = us::gov::cash::tx_t;
        using track_status_t = us::gov::engine::track_status_t;

        static constexpr const char* name{"w2w"};

        enum progress_t: uint8_t { //values coupled in fragment_w2w.java (android app)
            pg_idle,
            pg_error,
            pg_wait_4_outputs,
            pg_wait_4_inputs,
            pg_delivered,

            pg_numstates
        };

        static constexpr const char* pgstr[pg_numstates] = { "idle", "error", "wait_4_outputs", "wait_4_inputs", "delivered" };

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
            //svc_reset,

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
        static ko exec(istream&, traders_t&, wallet::local_api&);
        uint32_t trade_state_() const;
        void judge(const string& lang) override;


//        progress_t pg{pg_idle};
//        string pg_info;
//        us::gov::engine::daemon_t::evt_status_t gov_st{us::gov::engine::daemon_t::evt_unknown};

//        t1_t t1;
//        us::gov::cash::tx_t* tx{nullptr};
        mutable mutex mx;
    };

}

