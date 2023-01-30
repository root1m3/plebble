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
#include <us/gov/config.h>

#include "cert_index_t.h"

#define loglevel "wallet/trader/cert"
#define logclass "cert_index_t"
#include <us/gov/logs.inc>

using namespace std;
using c = us::wallet::trader::cert::cert_index_t;

void c::dump(ostream& os) const {
    string pfx = "    ";
    for (auto& i: *this) {
        os << i.first << ' ' << i.second << '\n';
//        i.second.dump(pfx, os);
    }
}

