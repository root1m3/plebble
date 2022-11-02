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
#include <us/gov/socket/daemon_t.h>
#include <us/gov/socket/rpc_daemon_t.h>
#include <us/gov/crypto/types.h>

#include "peer_t.h"
#include "daemon0_t.h"

namespace us::gov::id {

    struct daemon_t: base_ns::daemon_t, daemon0_t {
        using b = base_ns::daemon_t;

        daemon_t(channel_t channel, port_t, pport_t, int workers);

        bool is_duplicate(const pub_t&) const;
        void disconnect(const pub_t&, seq_t seq, const reason_t&);
        //socket::client* create_client(sock_t sock) override;

    public:
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;

    public:
        pport_t pport; //Public/Published/advertised/gossip'd TCP port for reaching up (router).

    };

}

