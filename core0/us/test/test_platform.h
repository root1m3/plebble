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
#pragma once
#include <iostream>
#include <vector>

namespace us{ namespace test {

    using namespace std;

    struct test_platform {
        test_platform(ostream& os);
        virtual ~test_platform();

        void print_stacktrace(ostream&out, unsigned int max_frames = 10);

        template<typename t>
        void check(const t& v, const t& expected) {
            if (v == expected) return;
            os << "Fail. Expected " << expected << ", got " << v << '\n';
            abort();
        }

        template<typename t>
        void check(const vector<t>& v, const vector<t>& expected) {
            if (v.size() != expected.size()) {
                os << "Fail. vector sizes differ. Expected " << expected.size() << ", got " << v.size() << '\n';
                abort();
            }
            if (v == expected) return;
            os << "Fail. content of vectors of size " << v.size() << " differ.\n";
            auto i = v.begin();
            auto e = expected.begin();
            while(i != v.end()) {
                os << +*i << ' ' << +*e << '\n';
                ++i; ++e;
            }
            abort();
        }

        void abort();
        void fail(const string& reason);

        ostream& os;
    };


}}




