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
#ifdef DEBUG
    #include <type_traits>
   struct assert_is_root_namespace;
   static_assert(std::is_same<assert_is_root_namespace, ::assert_is_root_namespace>::value, "Not root namespace. Check includes.");
#endif
#include "ripemd160.h"
#include "sha256.h"
#include "ec.h"

namespace us {

    using namespace std;

    using hasher_t = gov::crypto::ripemd160;
    using hash_t = hasher_t::value_type;
    using sigmsg_hasher_t = gov::crypto::sha256;
    using sigmsg_hash_t = sigmsg_hasher_t::value_type;
    using keys = gov::crypto::ec::keys;
    using keys_t = keys;
    using pubkey_t = keys_t::pub_t;
    using pub_t = pubkey_t;
    using privkey_t = keys_t::priv_t;
    using priv_t = privkey_t;
    using sig_t = gov::crypto::ec::sig_t;

}

namespace us { namespace gov { namespace crypto {

    using namespace us;

}}}

