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
#include "peer_t.h"
#include "types.h"
#include <us/gov/engine/protocol.h>

#define loglevel "gov/engine"
#define logclass "peer_t__sys"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::peer_t;

#include <us/api/generated/c++/gov/sys/cllr_rpc-impl>

bool c::process_work__sys(datagram* d) {
    using namespace protocol;
    switch(d->service) {
        #include <us/api/generated/c++/gov/sys/hdlr_svc-router>
    }
    return false;
}

#include <us/api/generated/c++/gov/sys/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/c++/gov/sys/hdlr_local-impl

ko c::handle_f1() {
    log("f1");

    // Implement here.

    return ok;
}

//-/----------------apitool - End of API implementation.

