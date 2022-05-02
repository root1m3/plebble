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
#include <us/gov/socket/datagram.h>

#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/chat_t.h>
#include <us/wallet/wallet/local_api.h>

#include <us/trader/r2r/bid2ask/types.h>
#include <us/trader/r2r/bid2ask/protocol.h>
#include <us/trader/workflow/consumer/docs.h>

namespace us { namespace trader { namespace r2r { namespace bid2ask { namespace bid {

    struct business_t;

    struct protocol: bid2ask::protocol {
        using b = bid2ask::protocol;
        using invoice_t = us::trader::workflow::consumer::invoice_t;
        using payment_t = us::trader::workflow::consumer::payment_t;
        using business_t = us::trader::r2r::bid2ask::bid::business_t;
        using tx_t = us::gov::cash::tx_t;

        static const char *KO_30291;

        protocol(business_t&);
        ~protocol() override {}

        ko trading_msg(peer_t&, svc_t, blob_t&&) override;
        void dump(ostream&) const override;
        void help_online(const string& indent, ostream&) const override;
        void help_onoffline(const string& indent, ostream&) const override;
        void list_trades_bit(ostream& os) const;
        const char* rolestr() const override;
        const char* peer_rolestr() const override;
        const char* get_name() const override;
        static constexpr auto name{"bid2ask"};
        string authorize_invoice(const string& txb58) const;
        chat_t::entry AI_chat(const chat_t&, peer_t&) override;
        void post_configure(ch_t&) override;
        payment_t* create_payment(const tx_t&, const string& msg) const;
        uint32_t trade_state_() const;
        ko exec_online(peer_t&, const string& cmd, ch_t&) override;
        bool requires_online(const string& cmd) const override;
        void judge(const string& lang) override;

    public:
        bool autopay{false};
        basket_t remote_stock;
    };

}}}}}

