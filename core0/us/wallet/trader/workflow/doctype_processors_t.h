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
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "doc_t.h"
#include "doc0_t.h"

namespace us::wallet::trader::workflow {

    using namespace std;
    using magic_t = us::wallet::trader::workflow::doc0_t::magic_t;

    struct doctype_processor_t: pair<string, string> {
        using b = pair<string, string>;
        using b::pair;

        void to_stream(ostream&) const;
        ko from_stream(istream&);
    };

    struct doctype_processors_t: map<magic_t, vector<doctype_processor_t>> { ///TODO: use unordered_map?
        doctype_processors_t();
        static constexpr uint8_t version{1};
        void add(magic_t, const string&, const string&);
        void add(magic_t m, doctype_processor_t&&);
        void to_stream(ostream&) const;
        ko from_stream(istream&);
        ko load(const string& filename);
        ko save(const string& filename);
    };

}

