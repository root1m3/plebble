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
#pragma once
#include <vector>
#include <us/gov/io/seriable_vector.h>
#include "section_t.h"

namespace us::gov::cash::tx {

    struct sections_t final: io::seriable_vector<section_t> {
        using b = io::seriable_vector<section_t>;

        void write_sigmsg(sigmsg_hasher_t&, const sigcode_t&) const;
        void write_pretty(const string& prefix, ostream&) const;
        bool check_amounts() const;
        const_iterator find(const hash_t& token) const;
        iterator find(const hash_t& token);
        uint64_t uniq() const { return empty() ? 0 : begin()->uniq(); }

    };

}

