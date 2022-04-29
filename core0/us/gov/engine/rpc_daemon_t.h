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
#include "net.h"
#include "rpc_peer_t.h"
#include <us/gov/socket/client.h>

namespace us::gov::engine {

    struct rpc_daemon_t: net::rpc_daemon_t {
        using b = net::rpc_daemon_t;

        rpc_daemon_t(channel_t channel, dispatcher_t*);
        ~rpc_daemon_t() override;

        socket::client* create_client() override;

        const rpc_peer_t& get_peer() const { return static_cast<const rpc_peer_t&>(*peer); }
        rpc_peer_t& get_peer() { return static_cast<rpc_peer_t&>(*peer); }

    public:
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;
    };

}

