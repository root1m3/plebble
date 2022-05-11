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
#include <vector>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/socket/types.h>

#include "cfg1.h"

namespace us::gov::io {

    using namespace std;

    struct cfg: cfg1 {
        using b = cfg1;
        using pubkeyh_t = crypto::ripemd160::value_type;

        cfg(const keys_t::priv_t& privk, const string& home, vector<hostport_t>&& seed_nodes);
        cfg(const cfg& other);
        ~cfg() override;

        static vector<pair<pubkeyh_t, hostport_t>> parse_nodes_file2(const string& absfile, channel_t);
        static void parse_nodes_file(vector<pair<host_t, port_t>>&, const string& absfile, channel_t);
        static hostport_t parse_host(const string& addrport);
        static pair<ko, cfg*> load(channel_t channel, const string& home, bool gen);
        vector<hostport_t> seed_nodes;
    };

    using cfg_daemon = cfg;

}

