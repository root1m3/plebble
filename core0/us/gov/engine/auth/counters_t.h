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

#if CFG_COUNTERS == 1
#include <us/gov/types.h>

namespace us { namespace gov { namespace engine { namespace auth {

    struct counters_t final {
        uint32_t basic_auth_completed{0};
        uint32_t not_authorized{0};
        uint32_t invalid_net_address{0};
        uint32_t sysop_connections{0};
        uint32_t node_connections{0};
        uint32_t out_connections{0};
        uint32_t hall_connections{0};

        void dump(ostream&) const;
    };

}}}}

#endif

