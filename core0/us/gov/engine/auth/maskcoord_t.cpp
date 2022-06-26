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
#include "maskcoord_t.h"
#include <us/gov/config.h>
#include <bitset>

#define loglevel "us/gov/engine/auth"
#define logclass "maskcoord_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::maskcoord_t;

void c::dump_1liner(ostream& os) const {
    if (mode == 0) {
        os << "{mask-coord " << bitset<32>(mask) << "-" << bitset<32>(coord) << '}';
    }
    else {
        os << "{maskbit " << bitset<32>(mask) << '}';
    }
}

bool c::operator == (const maskcoord_t& other) const {
    if (mask != other.mask) return false;
    if (coord != other.coord) return false;
    return true;
}

void c::clear() {
    mask = coord = 0;
}

ostream& operator << (ostream& os, const maskcoord_t& o) {
    o.dump_1liner(os);
    return os;
}

