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
#include "roles_t.h"

#include <us/gov/io/cfg0.h>
#include <us/gov/crypto/base58.h>

#include "trader_t.h"

#define loglevel "wallet/trader"
#define logclass "roles_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::roles_t;

void c::dump(const string& pfx, ostream& os) const {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        os << pfx << i.first << ' ' << i.second << '\n';
    }
}

