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
#include "tcp_addr.h"
#include "addr_t.h"

using namespace std;
using namespace us::gov::engine;
using c = us::gov::engine::addr_t;

c* c::from_stream(istream& is) {
    int n;
    is >> n;
    if (n == tcp_addr::id) return tcp_addr::from_stream(is);
    return 0;
}

ostream& operator << (ostream& os, const addr_t& o) {
    o.to_stream(os);
    return os;
}

