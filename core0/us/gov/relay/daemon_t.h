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
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>
#include <cassert>

#include <us/gov/crypto/ec.h>
#include <us/gov/peer/daemon_t.h>
#include <us/gov/socket/types.h>

#include "types.h"
#include "peer_t.h"

namespace us::gov::relay {

    struct daemon_t: base_ns::daemon_t {
        using b = base_ns::daemon_t;

        daemon_t(channel_t channel);
        daemon_t(channel_t channel, port_t, pport_t, uint8_t edges, uint8_t devices, int workers);
        daemon_t(const daemon_t&) = delete;
        ~daemon_t() override;

        daemon_t& operator = (const daemon_t&) = delete;

        void dump(const string& prefix, ostream&) const;
        int clique_send(int num, const peer_t* exclude, const datagram&);
        int clique_send(int num, const peer_t* exclude, datagram*);
        int grid_devices_send(const datagram&);
        virtual bool check_relay_permission(const hash_t& addr) const;
        //socket::client* create_client(sock_t sock) override { assert(false); return nullptr; }

        using filter_t = function<bool(const relay::peer_t&)>;
        struct push_item {
            push_item(datagram* dgram, const filter_t& filter): dgram(dgram), filter(filter) {}
            socket::datagram* dgram;
            const filter_t& filter;
        };
        int grid_devices_send(const vector<push_item>&);

    };

}

