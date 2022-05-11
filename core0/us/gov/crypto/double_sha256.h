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
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <cstring>

#include <us/gov/likely.h>

#include "sha256.h"
#include "ripemd160.h"

namespace us::gov::crypto {

    using namespace std;

    class double_sha256 {
    private:
        sha256 sha;
    public:
        using value_type = sha256::value_type;
        static const size_t output_size = sha256::output_size;
        void finalize(unsigned char hash[output_size]);
        void finalize(value_type& hash);
        void write(const unsigned char *data, size_t len);
        void write(const string& data);
        void write(const value_type& data);
        void write(const ripemd160::value_type& data);
        void write(bool);
        void reset();

    };

}

