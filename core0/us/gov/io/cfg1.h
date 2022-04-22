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
#include <string>
#include <utility>

#include <us/gov/config.h>
#include <us/gov/crypto/types.h>

#include "cfg0.h"
#include "types.h"

namespace us { namespace gov { namespace io {

    using namespace std;

    struct cfg1: cfg0 {
        using b = cfg0;

        static const char* KO_97832, *KO_30291, *KO_50493, *KO_60987;

        cfg1(const keys_t::priv_t& privk, const string& home);
        cfg1(const cfg1& other);
        ~cfg1() override;

        static pair<ko, keys_t::priv_t> load_sk(const string& home);
        static string k_file(const string& home);
        static ko write_k(const string& home, const keys_t::priv_t& priv);
        static pair<ko, cfg1*> load(const string& home, bool gen);

        keys_t keys;
    };

    using cfg_id = cfg1;

}}}

