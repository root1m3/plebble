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
#include <unordered_map>
#include <mutex>

#include <us/gov/types.h>
#include <us/gov/config.h>

#include "shell.h"

namespace us::gov::engine {

    struct daemon_t;
    struct peer_t;

    struct sysops_t final: unordered_map<peer_t*, shell> {
        sysops_t(daemon_t& d): d(d) {}
        ko exec(peer_t*, const string& cmd, ostream&);

    public:
        mutex mx;
        daemon_t& d;
    };

}

