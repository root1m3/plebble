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
#include <string>
#include <stdio.h>

#include <us/gov/config.h>

#include <crypto++/gcm.h>
#include <crypto++/aes.h>
#include <crypto++/osrng.h>

#include "ec.h"

namespace us { namespace gov { namespace crypto {

    using namespace std;
    using CryptoPP::AutoSeededRandomPool;
    using CryptoPP::GCM;
    using CryptoPP::AES;

    struct symmetric_encryption {
        using keys = ec::keys;
        using byte = unsigned char;

        static constexpr size_t key_size = 16;
        static constexpr size_t iv_size = 12;
        static constexpr int tag_size = 16;

        static size_t encmsg_max_sz(const size_t plaintext_sz);

        ko init(const keys::priv_t&, const keys::pub_t&);
        ko encrypt(const vector<unsigned char>& src, vector<unsigned char>& dest, size_t offset);
        ko decrypt(const unsigned char* p, size_t sz, vector<unsigned char>& dest);
        ko decrypt(const vector<unsigned char>& ciphertext, vector<unsigned char>& clear);

        AutoSeededRandomPool prng_;
        byte key_[key_size];
    };

}}}

