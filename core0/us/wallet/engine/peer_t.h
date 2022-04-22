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
#include <us/gov/relay/peer_t.h>
#include <us/gov/relay/pushman.h>
#include <us/wallet/wallet/algorithm.h>
#include "api.h"

namespace us { namespace wallet { namespace wallet {
        struct local_api;
}}}

namespace us { namespace wallet { namespace trader {
        struct trader_t;
        struct traders_t;
}}}

namespace us { namespace wallet { namespace engine {

    struct daemon_t;

    struct peer_t: gov::relay::peer_t, api {
        using b = gov::relay::peer_t;
        using w = wallet::algorithm;

        static const char* KO_10428;

        peer_t(daemon_t&, sock_t);
        ~peer_t() override;

        void verification_completed(pport_t, pin_t) override;
        bool authorize(const pub_t&, pin_t) override;
        bool process_work(datagram*) override;
        bool process_work__engine(datagram*);
        bool process_work__pairing(datagram*);
        bool process_work__wallet(datagram*);
        bool process_work__r2r(datagram*);
        void announce(pport_t) const;
        void disconnect_hilarious(datagram*);

    public:
        void schedule_push(socket::datagram*);
        ko push_KO(ko msg);
        ko push_KO(const hash_t& tid, ko msg);
        ko push_OK(const string& msg);
        ko push_OK(const hash_t& tid, const string& msg);

    public: //engine
        #include <us/api/generated/c++/wallet/engine/cllr_override>
        #include <us/api/generated/c++/wallet/engine/hdlr_override>
        #include <us/api/generated/c++/wallet/engine/hdlr_svc_handler-hdr>

    public: //pairing
        #include <us/api/generated/c++/wallet/pairing/cllr_override>
        #include <us/api/generated/c++/wallet/pairing/hdlr_override>
        #include <us/api/generated/c++/wallet/pairing/hdlr_svc_handler-hdr>

    public: //wallet
        #include <us/api/generated/c++/wallet/wallet/cllr_override>
        #include <us/api/generated/c++/wallet/wallet/hdlr_svc_handler-hdr>

    public: //r2r
        #include <us/api/generated/c++/wallet/r2r/cllr_override>
        #include <us/api/generated/c++/wallet/r2r/hdlr_override>
        #include <us/api/generated/c++/wallet/r2r/hdlr_svc_handler-hdr>

        static string lang_en;
        inline const string& get_lang() const { return lang_en; }

    public:
        function<void(pport_t)> post_auth;
        function<void()> shutdown;
        wallet::local_api* wallet{nullptr};
    };

}}}



