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
#include <us/gov/dfs/peer_t.h>
#include <us/gov/dfs/rpc_peer_t.h>
#include <us/gov/dfs/daemon_t.h>
#include <us/gov/dfs/rpc_daemon_t.h>
#include <us/gov/dfs/api.h>

namespace us::gov::engine::net {

    namespace ns = us::gov::dfs;
    using peer_t = ns::peer_t;
    using daemon_t = ns::daemon_t;
    using rpc_daemon_t = ns::rpc_daemon_t;
    using rpc_peer_t = ns::rpc_peer_t;

}

