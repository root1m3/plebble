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

#include <map>

#include <us/gov/socket/types.h>

namespace us::wallet::engine {

    struct svcfish_t final {
        using db_t = map<svc_t, svc_t>;

        svc_t from_prev(svc_t svc) const;
        svc_t to_prev(svc_t svc) const;


        db_t db01 {
            #include <us/api/generated/gov/c++/svc_from_prev_v>
            #include <us/api/generated/wallet/c++/svc_from_prev_v>
        };

        db_t db10 {
            #include <us/api/generated/gov/c++/svc_to_prev_v>
            #include <us/api/generated/wallet/c++/svc_to_prev_v>
        };

    };

}

