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
#include "sections_t.h"
#include <us/gov/cash/sigcode_t.h>
#include <us/gov/cash/types.h>
#include <us/gov/config.h>

#define loglevel "gov/cash/tx"
#define logclass "sections_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::tx::sections_t;

c::iterator c::find(const hash_t& token) {
    iterator i = begin();
    while (i != end()) {
        if (i->token == token) return i;
        ++i;
    }
    return i;
}

c::const_iterator c::find(const hash_t& token) const {
    const_iterator i = cbegin();
    while (i != cend()) {
        if (i->token == token) return i;
        ++i;
    }
    return i;
}

void c::write_sigmsg(sigmsg_hasher_t& h, const sigcode_t& sc) const {
    if (sc.empty()) return;
    auto sec = sc.begin();
    for (auto& i: *this) {
        i.write_sigmsg(h, *sec);
        ++sec;
        if (sec == sc.end()) return;
    }
}

void c::write_pretty(const string& prefix, ostream& os) const {
    int n = 0;
    for (auto& i: *this) {
        os << prefix << "section #" << n << ". ";
        if (i.token.is_zero()) {
            os << UGAS;
        }
        else {
            os << "coin " << i.token;
        }
        os << '\n';
        i.write_pretty(prefix + "  ", os);
        ++n;
    }
}

bool c::check_amounts() const {
    if (empty()) return false;
    for (auto& i: *this) {
        if (!i.check_amounts()) return false;
    }
    return true;
}

