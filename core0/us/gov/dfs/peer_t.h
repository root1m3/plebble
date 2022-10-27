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
#include <us/gov/config.h>

#include "api.h"
#include "types.h"

namespace us::gov::dfs {

    struct daemon_t;

    struct peer_t: base_ns::peer_t, api {
        using b = base_ns::peer_t;

        peer_t(daemon_t&, int sock);
        ~peer_t() override;

        bool process_work(datagram*) override;

        #ifdef has_us_gov_dfs_api
            #include <us/api/generated/gov/c++/dfs/cllr_override>
            #include <us/api/generated/gov/c++/dfs/hdlr_svc_handler-hdr>
            #include <us/api/generated/gov/c++/dfs/hdlr_override>
        #endif
    };

}

