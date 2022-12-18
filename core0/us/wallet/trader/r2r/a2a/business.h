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
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>

#include <us/gov/cash/tx_t.h>
#include <us/gov/io/factory.h>
#include <us/gov/relay/pushman.h>

#include <us/wallet/trader/business.h>
#include <us/wallet/trader/traders_t.h>

#include "protocol.h"

namespace us::wallet::trader::r2r::a2a {

    using namespace us::gov;

    struct business_t final: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using protocol = us::wallet::trader::r2r::a2a::protocol;

    public:
        business_t();
        ~business_t() override;

    private:
        protocol_factory_id_t protocol_factory_id() const;

    public:
        void register_factories(protocol_factories_t&) override;

    public:
        ko init(const string& r2rhome, protocol_factories_t&) override;

    public:
        string homedir() const override;
        pair<ko, trader::trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        bool invert(protocol_selection_t&) const override;
        void published_protocols(protocols_t&, bool inverse) const override;
        void exec_help(const string& prefix, ostream&) const override;
        ko exec(istream&, wallet::local_api&) override;

    };

}

