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

#include <iostream>
#include <cassert>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/cli/hmi.h>
#include <us/wallet/cli/hmi.h>

namespace us::test {

    int core0_main(int argc, char** argv);

    using namespace std;
    using hash_t = us::gov::crypto::ripemd160::value_type;

    struct govx_t: gov::cli::hmi {
        using b = gov::cli::hmi;

        using b::hmi;

        void on_stop() override {
            b::on_stop();
            if (_assert_on_stop) {
                cerr << "Unexpectedly hmi instance " << this << " stopped" << endl;
                assert(false);
            }
        }

        void setup_signals(bool) override {}

        bool _assert_on_stop = true;
    };

    struct walletx_t : wallet::cli::hmi {
        using b = wallet::cli::hmi;

        using b::hmi;

        void on_stop() override {
            b::on_stop();
            if (_assert_on_stop) {
                cerr << "Unexpectedly hmi instance " << this << " stopped" << endl;
                assert(false);
            }
        }

        void setup_signals(bool) override {}

        bool _assert_on_stop = true;
    };

    using cash_t = int64_t;

}

