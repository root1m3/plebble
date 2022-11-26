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
#include "api.h"
#include <us/gov/io/seriable.h>
#include <us/gov/socket/daemon0_t.h>

#include "peer_t.h"

#define loglevel "gov/relay"
#define logclass "peer_t"
#include "logs.inc"
#include <us/gov/socket/dto.inc>

using namespace us::gov::relay;
using c = us::gov::relay::peer_t;


bool peer_t::process_work(datagram* d) {
    assert(d != nullptr);
    #ifdef has_us_gov_relay_api
    using namespace us::gov::protocol;
    switch(d->service) {
        #include <us/api/generated/gov/c++/relay/hdlr_svc-router>
    }
    #endif
    return b::process_work(d);
}

#ifdef has_us_gov_relay_api

#include <us/api/generated/gov/c++/relay/hdlr_svc_handler-impl>


//------------------apitool - API Spec defined @ us/api/generated/gov/c++/relay/hdlr_local-impl

ko c::handle_push(push_in_dst_t&& o_in) {
    log("push");
    /// in:
    ///     hash_t tid;
    ///     uint16_t code;
    ///     blob_t blob;


    // Implement here.

    return KO_10210;
}

//-/----------------apitool - End of API implementation.


#include <us/api/generated/gov/c++/relay/cllr_rpc-impl>

#endif

