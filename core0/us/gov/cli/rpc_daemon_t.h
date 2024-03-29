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
#include <stack>

#include <us/gov/engine/rpc_daemon_t.h>
#include <us/gov/io/screen.h>
#include <us/gov/io/seriable.h>
#include <us/gov/crypto/types.h>

#include "rpc_peer_t.h"
#include "types.h"

namespace us::gov::cli {

    struct hmi;

    struct rpc_daemon_t: engine::rpc_daemon_t {
        using b = engine::rpc_daemon_t;
        using peer_type = rpc_peer_t;
        using request_data_t = peer_type::request_data_t;

        rpc_daemon_t(channel_t, const keys&, const shostport_t&, role_t, dispatcher_t*);
        rpc_daemon_t(hmi&, const keys&, const shostport_t&, role_t, dispatcher_t*);

    public:
        socket::client* create_client() override;

    public:
        ko connect() override;
        ko connect(pin_t pin, request_data_t&&);

    public:
        inline const rpc_peer_t& get_peer() const { return static_cast<const rpc_peer_t&>(*peer); }
        inline rpc_peer_t& get_peer() { return static_cast<rpc_peer_t&>(*peer); }

    public:
        void on_connect(socket::client&, ko) override;
        void on_stop() override;

    public:
        virtual void on_peer_disconnected(const reason_t&);
        virtual void on_I_disconnected(const reason_t&);
        virtual void verification_completed(bool verif_ok);
        virtual void verification_result(request_data_t&&);

    public:
        inline const keys_t& get_keys() const override { return id; }

    public:
        virtual void upgrade_software();

    public:
        static void apihelp(const string& prefix, ostream&);

    public:
        keys_t id;
        shostport_t shostport;
        role_t role;

        hmi* parent{nullptr};
    };

}

