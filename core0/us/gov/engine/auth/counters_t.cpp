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
#include "counters_t.h"
#if CFG_COUNTERS == 1

#define loglevel "gov/engine/auth"
#define logclass "counters_t"
#include <us/gov/engine/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::counters_t;
using us::ko;

void c::dump(ostream& os) const {
    os << "basic_auth_completed " << basic_auth_completed << '\n';
    os << "not_authorized " << not_authorized << '\n';
    os << "invalid_net_address " << invalid_net_address << '\n';
    os << "sysop_connections " << sysop_connections << '\n';
    os << "node_connections " << node_connections << '\n';
    os << "out_connections " << out_connections << '\n';
    os << "hall_connections " << hall_connections << '\n';
}

#endif

