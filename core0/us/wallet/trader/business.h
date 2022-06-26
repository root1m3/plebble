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
#include <us/gov/relay/pushman.h>
#include <us/gov/cash/tx_t.h>
#include <us/gov/crypto/ripemd160.h>

#include <us/wallet/wallet/local_api.h>

#include "protocol_selection_t.h"
#include "bookmarks_t.h"
#include "bootstrap/protocols_t.h"

namespace us::wallet::wallet {

    struct local_api;

}

namespace us::wallet::trader {

    using namespace us::gov;
    struct trader_protocol;
    struct traders_t;

    struct business_t {
        using pushman = us::gov::relay::pushman;
        using wallet_local_api = us::wallet::wallet::local_api;
        using trader_protocol = trader::trader_protocol;
        using protocol_selection_t = trader::protocol_selection_t;
        using bookmark_info_t = trader::bookmark_info_t;
        using protocols_t = trader::bootstrap::protocols_t;

        static const char* KO_50100; //exec ignored

        static constexpr int interface_version{8};
        business_t();
        virtual ~business_t() {}
        virtual ko init(const string& r2rhome);
        virtual string homedir() const = 0;
        virtual std::pair<ko, trader_protocol*> create_protocol(protocol_selection_t&& protocol_selection) = 0;
        virtual std::pair<ko, trader_protocol*> create_opposite_protocol(protocol_selection_t&& protocol_selection) = 0;
        virtual std::pair<ko, trader_protocol*> create_protocol() = 0;
        virtual void list_protocols(ostream&) const = 0; //human format
        virtual void invert(protocols_t&) const = 0;
        //virtual void to_stream_protocols(protocols_t&) const = 0;
        virtual void published_protocols(protocols_t&, bool inverse) const = 0;
        virtual void exec_help(const string& prefix, ostream&) const;
        virtual ko exec(istream&, traders_t&, wallet::local_api&);
        pair<protocol_selection_t, bookmark_info_t> bookmark_info() const;

    public:
        blob_t ico;
        string name;
        string home;
        string r2rhome;
    };

}

typedef us::wallet::trader::business_t* business_create_t(const char*, int ifversion);
typedef void business_destroy_t(us::wallet::trader::business_t*);

