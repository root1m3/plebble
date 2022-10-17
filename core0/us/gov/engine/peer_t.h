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

    struct peer_t: net::peer_t, api {
        using b = net::peer_t;

        enum stage_t {
            unknown = 0,
            sysop,
            out,
            hall,
            node,
            device,

            num_stages
        };
        constexpr static array<const char*, num_stages> stagestr = {"unknown", "sysop", "out", "hall", "node", "device"};

        peer_t(net_daemon_t&, int sock);
        ~peer_t() override;
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;
        void verification_completed(pport_t, pin_t) override;
        ko authorize(const pub_t&, pin_t) override;
        daemon_t& engine_daemon();
        const daemon_t& engine_daemon() const;

    public:
        bool process_work(datagram*) override;
        bool process_work__engine(datagram*);
        bool process_work__cash(datagram*);
        bool process_work__engine_auth(datagram*);
        bool process_work__traders(datagram*);
        bool process_work__sys(datagram*);

    public: //engine
        #include <us/api/generated/c++/gov/engine/cllr_override>
        #include <us/api/generated/c++/gov/engine/hdlr_override>
        #include <us/api/generated/c++/gov/engine/hdlr_svc_handler-hdr>

    public: //auth

        #include <us/api/generated/c++/gov/engine_auth/cllr_override>
        #include <us/api/generated/c++/gov/engine_auth/hdlr_override>
        #include <us/api/generated/c++/gov/engine_auth/hdlr_svc_handler-hdr>
    public: //cash

        #include <us/api/generated/c++/gov/cash/cllr_override>
        #include <us/api/generated/c++/gov/cash/hdlr_override>
        #include <us/api/generated/c++/gov/cash/hdlr_svc_handler-hdr>

    public: //traders
        #include <us/api/generated/c++/gov/traders/cllr_override>
        #include <us/api/generated/c++/gov/traders/hdlr_override>
        #include <us/api/generated/c++/gov/traders/hdlr_svc_handler-hdr>

    public: //sys
        #include <us/api/generated/c++/gov/sys/cllr_override>
        #include <us/api/generated/c++/gov/sys/hdlr_override>
        #include <us/api/generated/c++/gov/sys/hdlr_svc_handler-hdr>

    public:
        stage_t stage{unknown};
    };

}

