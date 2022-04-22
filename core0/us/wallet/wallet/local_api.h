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

#include <us/gov/config.h>
#include <us/gov/relay/pushman.h>

#include "handler_api.h"
#include "algorithm.h"
#include "types.h"

namespace us { namespace wallet { namespace engine {
        struct daemon_t;
}}}

namespace us { namespace wallet { namespace trader {
        struct traders_t;
}}}

namespace us { namespace wallet { namespace wallet {

    struct local_api: algorithm, handler_api {
        using w = algorithm;
        using daemon_t = engine::daemon_t;
        static const char* KO_10428;

        local_api(engine::daemon_t&, const string& home, const string& subhome, const hash_t& subhomeh, trader::endpoint_t&&);
        ~local_api() override;

        ko refresh_data();
        void dump(const string& pfx, ostream& os) const;
        pair<ko, affected_t> track_pay(const asa_t& asa_pay, const asa_t& asa_charge, tx_t&, string& progress);
        pair<ko, affected_t> analyze_pay(const asa_t& asa_pay, const asa_t& asa_charge, const tx_t&) const;
        ko track(const ts_t&, string& progress);

    public: //wallet
        #include <us/api/generated/c++/wallet/wallet/hdlr_override>

    public:
        relay::pushman::filter_t device_filter;
        string subhome;
        hash_t subhomeh;
        string lang{"en"};
        string home;
        trader::endpoint_t local_endpoint;
        daemon_t& daemon;
        #if CFG_LOGS == 1
            string logdir;
        #endif
    };

}}}



