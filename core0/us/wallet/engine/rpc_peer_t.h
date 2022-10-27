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
#include <us/gov/relay/rpc_peer_t.h>
#include "api.h"
#include "types.h"

namespace us { namespace wallet { namespace engine {

    struct rpc_peer_t: us::gov::relay::rpc_peer_t, caller_api  {
        using b = us::gov::relay::rpc_peer_t;

        using b::rpc_peer_t;
        bool process_work(datagram*) override;

        svc_t translate_svc(svc_t svc0, bool inbound) const override;

        #include <us/api/generated/wallet/c++/engine/cllr_override>
        #include <us/api/generated/wallet/c++/pairing/cllr_override>
        #include <us/api/generated/wallet/c++/wallet/cllr_override>
        #include <us/api/generated/wallet/c++/r2r/cllr_override>
    };

}}}

