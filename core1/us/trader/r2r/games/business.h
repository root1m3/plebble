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
#include "protocol.h"
#include <mutex>
#include <chrono>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>

#include <us/wallet/trader/traders_t.h>
#include <us/wallet/trader/business.h>
#include <us/gov/relay/pushman.h>

namespace us::wallet::trader {
    struct traders_t;
    struct trader_protocol;
}

namespace us::trader::r2r::games {

    using namespace us::gov;

    struct business_t final: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using trader_protocol = us::wallet::trader::trader_protocol;
        using traders_t = us::wallet::trader::traders_t;
        using wallet_local_api = us::wallet::wallet::local_api;
        using protocol = us::trader::r2r::games::protocol;

    public:
        business_t();
        ~business_t() override;

    public:
        ko init(const string& r2rhome, us::wallet::trader::traders_t::protocol_factories_t&) override;
        string homedir() const override;

    public:
        pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&&) override;
        pair<ko, trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        void invert(protocols_t&) const override;
        void published_protocols(protocols_t&, bool inverse) const override;
        void exec_help(const string& prefix, ostream&) const override;
        ko exec(istream&, traders_t&, wallet_local_api&) override;
    };

}

