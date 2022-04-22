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
#include <cassert>
#include <us/gov/id/rpc_daemon_t.h>
#include "types.h"

namespace us { namespace gov { namespace auth {

    struct rpc_daemon_t: base_ns::rpc_daemon_t {
        using b = base_ns::rpc_daemon_t;
        using b::rpc_daemon_t;

        socket::client* create_client() override { assert(false); return nullptr; }

    public:
        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;

    };

}}}

