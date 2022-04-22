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
#include "protocol.h"
#include <mutex>
#include <chrono>
#include <string>
#include <unordered_map>
#include <map>
#include <sstream>

#include <us/gov/cash/tx_t.h>
#include <us/gov/relay/pushman.h>

namespace us { namespace wallet { namespace trader {
    struct traders_t;
    struct trader_protocol;
}}}

namespace us { namespace wallet { namespace trader { namespace r2r { namespace w2w {

    using namespace us::gov;

    struct business_t final: us::wallet::trader::business_t {
        using b = us::wallet::trader::business_t;
        using protocol = us::wallet::trader::r2r::w2w::protocol;

        business_t();
        ~business_t() override;
        ko init(const string& r2rhome) override;
        string homedir() const override;

        pair<ko, trader::trader_protocol*> create_protocol(protocol_selection_t&&) override;
        pair<ko, trader::trader_protocol*> create_protocol() override;
        void list_protocols(ostream&) const override; //human format
        void to_stream_protocols(protocols_t&) const override; //serialization format
        void published_protocols(protocols_t&) const override; //serialization format
        void exec_help(const string& prefix, ostream&) const override;
        ko exec(istream&, traders_t&, wallet::local_api&) override;
    };

}}}}}

