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
#include "m_t.h"

#define loglevel "gov/cash"
#define logclass "m_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::m_t;

c::m_t() {
}

c::m_t(const c& other) {
    for (auto& i: other) {
        emplace(i);
    }
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

void c::hash_data(hasher_t& h) const {
    for (auto& i: *this) {
        h.write(i.first);
        h.write(i.second);
    }
}

void c::merge(const c& other) {
    for (auto i = other.begin(); i != other.end(); ++i) {
        auto j = emplace(*i);
        if (!j.second) {
            j.first->second += string("\n") + i->second;
        }
    }
}

void c::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << i.first << " -> " << i.second << '\n';
    }
}

