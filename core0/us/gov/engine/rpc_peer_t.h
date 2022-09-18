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
#include "net.h"
#include "api.h"

namespace us::gov::engine {

    struct daemon_t;
    struct net_daemon_t;

    struct rpc_peer_t: net::rpc_peer_t, caller_api {
        using b = net::rpc_peer_t;

        using b::rpc_peer_t;

        inline ko authorizeX(const pub_t&, pin_t) override { return ok; }
        bool process_work(datagram*) override;
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;

        #include <us/api/generated/c++/gov/engine/cllr_override>
        #include <us/api/generated/c++/gov/engine_auth/cllr_override>
        #include <us/api/generated/c++/gov/cash/cllr_override>
        #include <us/api/generated/c++/gov/traders/cllr_override>
        #include <us/api/generated/c++/gov/sys/cllr_override>
    };

}

