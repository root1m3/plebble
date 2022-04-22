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
#include "double_sha256.h"
#include "endian_rw.h"
#include "base58.h"
#include <us/gov/likely.h>
#include <cstring>

using namespace us::gov::crypto;
using namespace std;

using c = us::gov::crypto::double_sha256;

void c::finalize(unsigned char hash[output_size]) {
    unsigned char buf[sha.output_size];
    sha.finalize(buf);
    sha.reset();
    sha.write(buf, sha.output_size);
    sha.finalize(hash);
}

void c::finalize(value_type& hash) {
    unsigned char buf[sha.output_size];
    sha.finalize(buf);
    sha.reset();
    sha.write(buf, sha.output_size);
    sha.finalize(hash);
}

void c::write(const unsigned char *data, size_t len) {
    if(likely(len > 0)) sha.write(data, len);
}

void c::write(const string&data) {
    if(unlikely(data.empty())) return;
    sha.write(reinterpret_cast<const unsigned char*>(&data[0]), data.size());
}

void c::write(const value_type& data) {
    write(&data[0], output_size);
}

void c::write(const ripemd160::value_type& data) {
    write(&data[0], ripemd160::output_size);
}

void c::write(bool data) {
    write(reinterpret_cast<const unsigned char*>(&data), sizeof(data));
}

void c::reset() {
    sha.reset();
}

