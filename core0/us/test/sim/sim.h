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

#include <us/gov/crypto/ripemd160.h>
#include <random>

#define TX_SER_SZ 600 //avg size in bytes of a serialized transaction
#define DELTAS_SZ 10000 //avg size in bytes of a delta packet (part of consensus)
#define VOTE_SZ 160 //avg size in bytes of a vote packet (part of consensus)

#define SPLIT_SZ 32

namespace us::sim {

    struct clock {


        uint64_t time{0};
        static clock instance;
    };

    using hash_t = us::gov::crypto::ripemd160::value_type;
    using address_t = hash_t;

    static mt19937 rng(time(0));
    static uniform_int_distribution<mt19937::result_type> distu64;


    void sim_main();
}

