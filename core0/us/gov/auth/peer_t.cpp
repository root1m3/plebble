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
#include "peer_t.h"
#include <us/gov/socket/daemon0_t.h>

#define loglevel "gov/auth"
#define logclass "peer_t"
#include "logs.inc"

using namespace us::gov::auth;
using c = us::gov::auth::peer_t;

constexpr array<const char*, c::num_stages> c::stagestr;

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

void c::verification_completed(pport_t rpport, pin_t pin) {
    log("verification_completed", endpoint(), rpport, sock);
    if (unlikely(!verification_is_fine())) {
        #if CFG_COUNTERS == 1
            ++counters.failed_verifications;
        #endif
        log("verification_not_fine");
        return;
    }
    #if CFG_COUNTERS == 1
        ++counters.successful_verifications;
    #endif
    auto r = authorize(pubkey, pin);
    if (unlikely(is_ko(r))) {
        #if CFG_COUNTERS == 1
            ++counters.failed_authorizations;
        #endif
        log("authorization denied", pubkey, r);
        return;
    }
    #if CFG_COUNTERS == 1
        ++counters.successful_authorizations;
    #endif
    stage = authorized;
    log("stage", stagestr[stage]);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "  auth: stage " << stagestr[stage] << endl;
}

void c::dump_all(const string& prefix, ostream& os) const {
    dump(prefix, os);
    b::dump_all(prefix, os);
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "successful_verifications " << successful_verifications << '\n';
        os << "failed_verifications " << failed_verifications << '\n';
        os << "successful_authorizations " << successful_authorizations << '\n';
        os << "failed_authorizations " << failed_authorizations << '\n';
    }
#endif

bool c::process_work(datagram* d) {
    assert(d != nullptr);
    #ifdef has_us_gov_auth_api
    using namespace us::gov::protocol;
    switch (d->service) {
        #include <us/api/generated/c++/gov/auth/hdlr_svc-router>
    }
    #endif
    return b::process_work(d);
}

#ifdef has_us_gov_auth_api

    #include <us/api/generated/c++/gov/auth/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/c++/gov/auth/hdlr_local-impl

ko c::handle_f1() {
    log("f1");
    // Implement here.

    return KO_10210;
}

//-/----------------apitool - End of API implementation.


    #include <us/api/generated/c++/gov/auth/cllr_rpc-impl>

#endif

