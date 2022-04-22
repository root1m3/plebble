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
#include "txlog_item_t.h"

#define loglevel "wallet/trader/r2r/w2w"
#define logclass "txlog_item_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::r2r::w2w::txlog_item_t;
using namespace std;

c::txlog_item_t(const string& io_summary, uint16_t state_id, const string& state, tx_t* inv, tx_t* pay): io_summary(io_summary), inv(inv), pay(pay), state_id(state_id), state(state) {
}

string c::title() const {
    ostringstream os;
    os << '[' << state_id << ' ' << state << "] " << '\n';
    os << io_summary << '\n';
    return os.str();
}


