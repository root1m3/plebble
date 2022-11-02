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
#include <us/gov/io/seriable.h>
#include <us/gov/socket/types.h>
#include <us/gov/config.h>

#include "ip4_endpoint_t.h"

namespace us::wallet::engine {

    struct wallet_connection_t final_: gov::io::seriable {

    public:
        wallet_connection_t();
        wallet_connection_t(const string& nm, const ip4_endpoint_t& ep);
        wallet_connection_t(uint64_t ts_, const string& addr_, const string& subhome, const string& nm, const string& ssid_, const ip4_endpoint_t&);
        wallet_connection_t(const wallet_connection_t& other);

    public:
        wallet_connection_t copy() const;
        string log_string() const;
        ko set_endpoint(const ip4_endpoint_t&);
        string get_title() const;
        void dump(ostream&) const;

    public: //serialization blob
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string name_;
        string ssid;
        string addr;
        string subhome;
        ip4_endpoint_t ip4_endpoint;
        uint64_t ts{0};
    };

}

