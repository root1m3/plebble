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
#include <array>

#include <us/gov/id/peer_t.h>
#include <us/gov/config.h>
#include <us/gov/types.h>

#include "api.h"
#include "types.h"

namespace us::gov::auth {

    struct peer_t: base_ns::peer_t, api {
        using b = base_ns::peer_t;

        enum stage_t {
            denied = 0,
            authorized,
            num_stages
        };

        constexpr static array<const char*, num_stages> stagestr = {"denied", "authorized"};

        using b::peer_t;

    public:
        bool process_work(datagram*) override;

    public:
        void verification_completed(pport_t, pin_t) override;
        virtual bool authorize(const pub_t&, pin_t) = 0;

    public:
        void dump_all(const string& prefix, ostream&) const override;
        void dump(const string& prefix, ostream&) const;

    public:
    #ifdef has_us_gov_auth_api
        #include <us/api/generated/c++/gov/auth/hdlr_override>
        #include <us/api/generated/c++/gov/auth/hdlr_svc_handler-hdr>
        #include <us/api/generated/c++/gov/auth/cllr_override>
    #endif

    public:
    #if CFG_COUNTERS == 1
        struct counters_t {
           uint32_t successful_verifications{0};
           uint32_t failed_verifications{0};
           uint32_t successful_authorizations{0};
           uint32_t failed_authorizations{0};

           void dump(ostream&) const;
        };
        static counters_t counters;
    #endif

    public:
        stage_t stage{denied};
    };

}

