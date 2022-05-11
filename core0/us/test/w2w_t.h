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
#include "r2r_t.h"
#include <us/wallet/wallet/txlog_t.h>

namespace us { namespace test {

    using namespace std;

    struct network;
    struct node;

    struct w2w_t: r2r_t {
        using b = r2r_t;
        using track_t = us::wallet::wallet::track_t;

        w2w_t(network& n): b(n) {}

        track_t init_transfer(node& sender, node& rcpt);
        void cancel_transfer_by_sender1(node& sender, node& rcpt, track_t track);
        void cancel_transfer_by_sender2(node& sender, node& rcpt, track_t track);
        void confirm_transfer(node& sender, node& rcpt, track_t track);

        void test(node&, node&);
        void run();

        hash_t trade_id{0};

    };

}}

