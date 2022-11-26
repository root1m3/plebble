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
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include <us/gov/config.h>
#include <us/gov/types.h>
#include <us/gov/io/factory.h>
#include <us/gov/relay/pushman.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/crypto/ripemd160.h>

#include "trader_protocol.h"
#include "protocol_selection_t.h"
#include "bookmarks_t.h"
#include "bootstrap/protocols_t.h"

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::wallet {
    struct local_api;
}

namespace us::wallet::trader {

    using namespace us::gov;
    struct trader_protocol;
    struct traders_t;

    struct business_t {
        using pushman_t = us::gov::relay::pushman;
        using wallet_local_api = us::wallet::wallet::local_api;
        using trader_protocol = trader::trader_protocol;
        using protocol_selection_t = trader::protocol_selection_t;
        using bookmark_info_t = trader::bookmark_info_t;
        using protocols_t = trader::bootstrap::protocols_t;

        using protocol_factories_t = us::wallet::trader::trader_protocol::factories_t;
        using protocol_factory_t = us::wallet::trader::trader_protocol::factory_t;
        using protocol_factory_id_t = us::wallet::trader::trader_protocol::factory_id_t;

        static const char* KO_50100; //exec ignored

        static constexpr int interface_version{11};

    public:
        business_t();
        virtual ~business_t();

        virtual void register_factories(protocol_factories_t&) = 0;

    public:
        virtual ko init(const string& r2rhome, protocol_factories_t&);

    public:
        virtual string homedir() const = 0;
        virtual std::pair<ko, trader_protocol*> create_protocol() = 0;
        virtual void list_protocols(ostream&) const = 0; //human format
        virtual bool invert(protocol_selection_t&) const = 0;

        virtual void published_protocols(protocols_t&, bool inverse) const = 0;
        virtual void exec_help(const string& prefix, ostream&) const;
        virtual ko exec(istream&, wallet_local_api&);
        pair<protocol_selection_t, bookmark_info_t> bookmark_info() const;
        protocol_selection_t protocol_selection() const;
        void dump(const string& prefix, ostream&) const;

    public:
        using factories_t = us::gov::io::factories_t<business_t>;
        using factory_t = us::gov::io::factory_t<business_t>;
        using factory_id_t = protocol_selection_t;

    public:
        blob_t ico;
        string name;
        string home;
        string r2rhome;
    };

}


