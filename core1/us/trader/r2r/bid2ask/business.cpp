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
#include "business.h"
#include "protocol.h"

#define loglevel "trader/r2r/bid2ask"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::business_t;

c::business_t() {
}

c::~business_t() {
}

bool c::invert(protocol_selection_t& i) const {
    if (i.first != protocol::name) return false;
    if (i.second == "ask") {
        i.second = "bid";
        return true;
    }
    if (i.second == "bid") {
        i.second = "ask";
        return true;
    }
    return false;
}

