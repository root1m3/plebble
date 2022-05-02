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
#include "outputs_t.h"

#define loglevel "gov/cash/tx"
#define logclass "outputs_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::tx::outputs_t;
using us::ko;

cash_t c::total() const {
    cash_t x = 0;
    for (auto& i: *this) {
        x += i.amount;
    }
    return x;
}

void c::write_sigmsg(sigmsg_hasher_t& h, const sigcode_section_t& sc) const {
    for (auto& i: sc.outputs) {
        if (i >= size()) continue;
        auto it = begin();
        advance(it, i);
        it->write_sigmsg(h);
    }
}

void c::write_pretty(const string& prefix, ostream& os) const {
    int n = 0;
    for (auto& i: *this) {
        os << prefix << "output #" << n << ":\n";
        i.write_pretty(prefix + "  ", os);
        ++n;
    }
}

