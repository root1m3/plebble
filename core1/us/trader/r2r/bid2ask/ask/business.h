//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
#include <map>
#include <sstream>
#include <mutex>
#include <chrono>
#include <fstream>
#include <unordered_map>

#include <us/gov/cash/tx_t.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>

#include <us/trader/r2r/bid2ask/types.h>
#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/workflow/consumer/basket.h>

#include "protocol.h"

namespace us { namespace trader { namespace r2r { namespace bid2ask { namespace ask {

    struct business_t: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using ch_t = us::wallet::trader::ch_t;
        using catalogue_t = us::trader::workflow::consumer::catalogue_t;
        using basket_t = us::trader::workflow::consumer::basket_t;
        using protocol = ask::protocol;
        using tx_t = us::gov::cash::tx_t;

        business_t();
        ~business_t() override;
        ko init(const string& r2rhome) override;
        string homedir() const override;

        void fill_stock();
        ko load_coinsx();
        pair<ko, tx_t*> tx_charge_pay(us::wallet::wallet::local_api&, cash_t recv_amount, cash_t send_amount) const;
        pair<ko, us::wallet::trader::trader_protocol*> create_protocol(protocol_selection_t&&) override;
        pair<ko, us::wallet::trader::trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        void to_stream_protocols(protocols_t&) const override;
        void published_protocols(protocols_t&) const override;
        catalogue_t* catalogue(const string& lang);

    public:
        basket_t stock;
        hash_t address;
        hash_t recv_coin;
        hash_t send_coin;
    };

}}}}}

