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
#include "protocol.h"
#include <us/gov/io/seriable.h>
#if __has_include(<us/api/generated/gov/c++/auth/svc>)
    #define has_us_gov_auth_api
#endif

namespace us::gov::auth {

    using namespace gov::io;

    #ifdef has_us_gov_auth_api

        struct dto {
            using readable = us::gov::io::readable;
            using writable = us::gov::io::writable;

            #include <us/api/generated/gov/c++/auth/cllr_dto-hdr>
            #include <us/api/generated/gov/c++/auth/hdlr_dto-hdr>
        };

        struct caller_api: dto {
            #include <us/api/generated/gov/c++/auth/cllr_purevir>
        };

        struct api: caller_api {
            #include <us/api/generated/gov/c++/auth/hdlr_purevir>
        };

    #else

        struct api {};

    #endif

}

