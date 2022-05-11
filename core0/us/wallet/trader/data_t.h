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
#include <map>
#include <string>
#include <iostream>
#include <mutex>
#include <vector>

#include <us/gov/ko.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/io/seriable_vector.h>
#include <us/gov/io/seriable_map.h>

namespace us::wallet::trader {

    struct trader_t;

    struct data_t final: map<string, string> {
        using b = map<string, string>;

        struct diff_line {
            char action;
            string value;
        };

        struct diff_t: map<string, diff_line> {
            void filter(const string&);
            void dump(const string& pfx, ostream&) const;
            int seq{0};
        };

        data_t();
        data_t(const data_t&);
        data_t(data_t&&);
        ~data_t();

        data_t& operator = (const data_t&);

        bool operator == (const data_t&) const;
        bool operator != (const data_t&) const;

        void dump(const string& pfx, ostream&) const;
        void dump_(const string& pfx, ostream&) const;

        diff_t get_diff(const data_t& other) const;
        ko apply(const diff_t&);

        template<typename t>
        void add(const string& key, const t& value) {
            ostringstream os;
            os << value;
            emplace(key, value);
        }

    public:
        ko from(const string&);
        ko from(istream&);
        mutable mutex mx;
    };

    template<> void data_t::add(const string& key, const string& value);

}

