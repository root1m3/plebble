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
#include "sensors_t.h"

#define loglevel "gov/sys"
#define logclass "sensors_t"
#include <us/gov/logs.inc>

using c = us::gov::sys::sensors_t;
using namespace us::gov;
using namespace std;

void c::dump(const string& prefix, ostream& os) const {
    string pfx = prefix + "    ";
    for (auto& i: *this) {
        os << prefix << "node " << i.first << '\n';
        i.second.dump(pfx, os);
    }
}

