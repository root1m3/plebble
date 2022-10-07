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
#include <map>
#include <sstream>
#include <mutex>
#include <chrono>
#include <fstream>
#include <unordered_map>

#include <us/gov/cash/tx_t.h>
#include <us/gov/io/factory.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/workflow/workflow_t.h>

#include <us/trader/r2r/bid2ask/types.h>
#include <us/trader/r2r/bid2ask/business.h>
#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/workflow/consumer/basket.h>

#include "protocol.h"

namespace us::trader::r2r::bid2ask::ask {

    struct business_t: us::trader::r2r::bid2ask::business_t {
        using b = us::trader::r2r::bid2ask::business_t;
        using protocol = ask::protocol;

    public:
        business_t();
        ~business_t() override;

    public:
        protocol_factory_id_t protocol_factory_id() const;
        void register_factories(protocol_factories_t&) override;

    public:
        ko init(const string& r2rhome, us::wallet::trader::traders_t::protocol_factories_t&) override;

    public:
        string homedir() const override;
        void fill_stock();
        ko load_coinsx();
        pair<ko, tx_t*> tx_charge_pay(us::wallet::wallet::local_api&, cash_t recv_amount, cash_t send_amount) const;
        pair<ko, us::wallet::trader::trader_protocol*> create_opposite_protocol(protocol_selection_t&&) override;
        pair<ko, us::wallet::trader::trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        void invert(protocols_t&) const override;
        void published_protocols(protocols_t&, bool inverse) const override;
        catalogue_t* catalogue(const string& lang);

        //using workflow_factories_t = us::gov::io::factories_t<workflow_t>;
        //using workflow_factory_t = us::gov::io::factory_t<workflow_t>;

    public:
        basket_t stock;
        hash_t address;
        hash_t recv_coin;
        hash_t send_coin;

    };

}

