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
#include <us/gov/io/seriable.h>
#include "protocol.h"

#if __has_include(<us/api/generated/c++/gov/peer/svc>)
    #define has_us_gov_peer_api
#endif

namespace us::gov::peer {

    using namespace gov::io;

    #ifdef has_us_gov_peer_api
        struct dto {
            #include <us/api/generated/c++/gov/peer/cllr_dto-hdr>
            #include <us/api/generated/c++/gov/peer/hdlr_dto-hdr>
        };

        struct caller_api: dto {
            #include <us/api/generated/c++/gov/peer/cllr_purevir>
        };

        struct api: caller_api {
            #include <us/api/generated/c++/gov/peer/hdlr_purevir>
        };

    #else

        struct api {};

    #endif

}

