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
#include <us/test/r2r_t.h>
#include "network.h"

namespace us::test {
    using namespace std;

    struct bid2ask_t: r2r_t {
        using b = r2r_t;

        bid2ask_t(network& n): b(n) {}

        void test_0(node&, node&);
        void test_0_0(node&, node&);
        void test_0_ping(node&, node&);
        void test_0_1(node&, node&);
        void test_0_1_1(node&, node&);
        void test_0_1_2(node&, node&);
        bool test_0_1_2_w(node&, node&);
        void test_0_1_2_1(node&, node&);
        void test_0_1_3(node&, node&);
        void test_0_2(node&, node&);
        void test_0_3(node&, node&);
        void test_1(node&, node&);
        void test_3(node&, node&);
        void test_4(node&, node&);
        void test_5(node&, node&);
        void test_6(node&, node&);
        void test_7(node&, node&);
        void test_8(node&, node&);
        void test_9(node&, node&);
        void test_10(node&, node&);
        void test_11(node&, node&);
        void test_12(node&, node&);
        void test_13(node&, node&);
        void test_14(node&, node&);
        void test_15(node&, node&);
        void test_16(node&, node&);
        void test_17(node&, node&);
        void test_18(node&, node&);
        void test_19(node&, node&);
        void test_20(node&, node&);
        void test_21(node&, node&);
        void test_22(node&, node&);
        void test_23(node&, node&);
        void check_local_remote(node& n, const string& var);
        void check_wf_data(node& n);
        void check_data_fields(node& n);
        void test_24(node&, node&);
        void test_25(node&, node&);
        void test_26(node&, node&);
        void test_end(node&, node&);

        void test(node&, node&);
        void run();

        hash_t trade_id{0};

    };

}

