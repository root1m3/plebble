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

#include <us/gov/relay/peer_t.h>
#include <us/gov/relay/pushman.h>

#include <us/wallet/wallet/algorithm.h>

#include "api.h"

namespace us::wallet::wallet {
        struct local_api;
}

namespace us::wallet::trader {
        struct trader_t;
        struct traders_t;
}

namespace us::wallet::engine {

    struct daemon_t;

    struct peer_t: gov::relay::peer_t, api {
        using b = gov::relay::peer_t;
        using w = wallet::algorithm;

        peer_t(daemon_t&, sock_t);
        ~peer_t() override;

    public:
        ko verification_completed(pport_t, pin_t, request_data_t&) override;
        ko authorize(const pub_t&, pin_t, request_data_t&) override;
        bool process_work(datagram*) override;
        bool process_work__engine(datagram*);
        bool process_work__pairing(datagram*);
        bool process_work__wallet(datagram*);
        bool process_work__r2r(datagram*);
        void announce(pport_t) const;
        void disconnect_hilarious(datagram*);
        svc_t translate_svc(svc_t svc0, bool inbound) const override;
        void verification_result(request_data_t&&) override {}

    public: //engine
        #include <us/api/generated/wallet/c++/engine/cllr_override>
        #include <us/api/generated/wallet/c++/engine/hdlr_override>
        #include <us/api/generated/wallet/c++/engine/hdlr_svc_handler-hdr>

    public: //pairing
        #include <us/api/generated/wallet/c++/pairing/cllr_override>
        #include <us/api/generated/wallet/c++/pairing/hdlr_override>
        #include <us/api/generated/wallet/c++/pairing/hdlr_svc_handler-hdr>

    public: //wallet
        #include <us/api/generated/wallet/c++/wallet/cllr_override>
        #include <us/api/generated/wallet/c++/wallet/hdlr_svc_handler-hdr>

    public: //r2r
        #include <us/api/generated/wallet/c++/r2r/cllr_override>
        #include <us/api/generated/wallet/c++/r2r/hdlr_override>
        #include <us/api/generated/wallet/c++/r2r/hdlr_svc_handler-hdr>

    public:
        void upgrade_software() override;

    public:
        static string lang_en;
        inline const string& get_lang() const { return lang_en; }

    public:
        function<void(pport_t)> post_auth;
        function<void()> shutdown;
        wallet::local_api* local_w{nullptr}; //devices: local wallet operated as owner;
    };

}

