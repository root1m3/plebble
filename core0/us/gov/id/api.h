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

#include <us/gov/io/seriable_map.h>
#include <us/gov/crypto/ec.h>

#include "protocol.h"
#include "types.h"

#if __has_include(<us/api/generated/gov/c++/id/svc>)
    #define has_us_gov_id_api
#endif

namespace us::gov::id {

    using namespace us;
    using namespace us::gov::io;
    using sig_t = us::gov::crypto::ec::sig_t;
    using sig_der_t = us::gov::crypto::ec::sig_der_t;

    enum role_t: uint8_t {
        role_peer = 0,
        role_sysop = 1,
        role_device = 2,
        num_roles
    };

    using request_data_t = string;


    #ifdef has_us_gov_id_api

        struct dto {
            #include <us/api/generated/gov/c++/id/cllr_dto-hdr>
            #include <us/api/generated/gov/c++/id/hdlr_dto-hdr>
        };

        struct caller_api: dto {
            #include <us/api/generated/gov/c++/id/cllr_purevir>
        };

        struct api: caller_api {
            #include <us/api/generated/gov/c++/id/hdlr_purevir>
        };

    #else

        struct api {};

    #endif

}

