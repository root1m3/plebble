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
#include "cat.h"
#include <us/wallet/protocol.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/params_t.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/trader/workflow/types.h>
#include <fstream>

#define loglevel "trader/workflow/consumer"
#define logclass "cat_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow;
using c = us::trader::workflow::consumer::cat_t;

c::cat_t() {
}

c::~cat_t() {
}

void c::dump(ostream& os) const {
    for (auto& i: *this) {
        os << i.first << ' ' << i.second.first << ' ' << i.second.second << '\n';
    }
}

string c::fields() {
    return "product price reward";
}

