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
#include <mutex>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>

#include <us/gov/cash/tx_t.h>

#include <us/trader/r2r/bid2ask/types.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>

#include <us/trader/r2r/bid2ask/protocol.h>

namespace us::trader::r2r::bid2ask::ask {

    struct business_t;

    struct protocol: bid2ask::protocol {
        using b = bid2ask::protocol;
        using business_t = us::trader::r2r::bid2ask::ask::business_t;
        using tx_t = us::gov::cash::tx_t;

        static constexpr const char* name{"bid2ask"};

        protocol(business_t&);
        ~protocol() override;

    public:
        ko trading_msg(peer_t&, svc_t, blob_t&&) override;
        void dump(ostream&) const override;
        void help_online(const string& indent, ostream&) const override;
        const char* rolestr() const override;
        const char* peer_rolestr() const override;
        const char* get_name() const override;
        ko exec_online(peer_t&, const string& cmd, ch_t&) override;
        bool requires_online(const string& cmd) const override;
        blob_t push_payload(uint16_t pc) const override;
        chat_t::entry AI_chat(const chat_t&, peer_t&) override;
        ko workflow_item_requested(workflow_item_t&, peer_t&, ch_t&) override;
        ko on_receive(peer_t&, workflow_item_t&, workflow_doc0_t*, ch_t&) override;
        void post_configure(ch_t&) override;
        bool on_signal(int sig, ostream&) override;
        void sig_reload(ostream&);
        void ensure_catalogue(ch_t&);
        void judge(const string& lang) override;
        string check_KYC() const;
        string check_KYC(const hash_t& product) const;
        ko select(peer_t&, const hash_t& product, int number_of_units, bool superuser, ch_t& ch);
        void list_trades_bit(ostream&) const;
        invoice_t* create_invoice(const string& lang, const basket_t&, const tx_t&) const;
        receipt_t* create_receipt(const string& lang, const basket_t&, const tx_t&, const string& msg, const string& info) const;
        bool enabled_select{true};
    };

}

