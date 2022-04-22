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
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cassert>
#include <array>
#include <unordered_set>
#include <us/gov/config.h>
#include <us/gov/auth/peer_t.h>
#include "api.h"
#include "types.h"

namespace us { namespace gov { namespace peer {

    struct daemon_t;

    struct peer_t: base_ns::peer_t, api {
        using b = base_ns::peer_t;

        enum stage_t {
            stage_disconnected = 0,
            stage_connected,
            stage_exceed_latency,
            stage_service,
            stage_disconnecting,
            num_stages
        };
        constexpr static array<const char*, num_stages> stagestr = {"disconnected", "connected", "latency", "service", "disconnecting"};
        static chrono::seconds auth_window;
        constexpr static ts_t maxidle_ms{180000};
        static const char* finished_reason_1;

        peer_t(daemon_t&, sock_t sock);
        ~peer_t() override;

        ko connect(const hostport_t&, pport_t, pin_t, role_t, bool block) final override;
        using b::disconnect;
        void disconnectx(channel_t, seq_t, const reason_t&) override;
        void dump_all(const string& prefix, ostream&) const override;
        pair<security_level_t, string> security_level() const override;
        bool check_auth_not_granted() const;
        bool check_idle() const;
        void dump(const string& prefix, ostream&) const;
        bool authorize(const pub_t&, pin_t) override;
        bool process_work(datagram*) override;

    public:
        #ifdef has_us_gov_peer_api
            #include <us/api/generated/c++/gov/peer/hdlr_override>
            #include <us/api/generated/c++/gov/peer/hdlr_svc_handler-hdr>
            #include <us/api/generated/c++/gov/peer/cllr_override>
        #endif

    public:
        stage_t stage;
    };

}}}

