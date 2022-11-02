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

    struct rpc_peer_t: net::rpc_peer_t, caller_api {
        using b = net::rpc_peer_t;

        using b::rpc_peer_t;

        inline ko authorize(const pub_t&, pin_t, request_data_t&) override { return ok; }
        bool process_work(datagram*) override;
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;
        svc_t translate_svc(svc_t svc0, bool inbound) const override;

        #include <us/api/generated/gov/c++/engine/cllr_override>
        #include <us/api/generated/gov/c++/engine_auth/cllr_override>
        #include <us/api/generated/gov/c++/cash/cllr_override>
        #include <us/api/generated/gov/c++/traders/cllr_override>
        #include <us/api/generated/gov/c++/sys/cllr_override>
    };

}

