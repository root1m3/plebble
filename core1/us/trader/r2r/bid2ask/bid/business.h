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

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/business.h>

#include <us/trader/r2r/bid2ask/protocol.h>
#include <us/trader/r2r/bid2ask/types.h>

#include "protocol.h"

namespace us::trader::r2r::bid2ask::bid {

    struct business_t: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using wallet_local_api = us::wallet::wallet::local_api;
        using trader_protocol = us::wallet::trader::trader_protocol;
        using protocol = bid::protocol;

    public:
        business_t();
        ~business_t() override;
        ko init(const string& r2rhome) override;
        string homedir() const override;

    public:
        pair<ko, trader_protocol*> create_protocol(protocol_selection_t&&) override;
        pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&&) override;
        pair<ko, trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        void invert(protocols_t&) const override;
        void published_protocols(protocols_t&, bool inverse) const override;

    };

}

