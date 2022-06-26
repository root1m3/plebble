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

#include <us/gov/crypto/ec.h>
#include <vector>
#include <stdio.h>

namespace us::test {
    using namespace std;
    using keys = us::gov::crypto::ec::keys;

    void test_symmetric_encryption();
    void test_encrypt_decrypt(string);
    void test_encrypt_decrypt(string, const keys::priv_t&, const keys::pub_t&, const keys::priv_t&, const keys::pub_t&, bool should_work=true);

}

