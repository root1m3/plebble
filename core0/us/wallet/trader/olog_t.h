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

#include <mutex>
#include <fstream>
#include <condition_variable>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/socket/client.h>
#include <us/gov/io/blob_writer_t.h>

#include <us/wallet/engine/peer_t.h>

#include "endpoint_t.h"

namespace us::wallet::trader {

    using namespace std;

    struct olog_t {
        using hash_t = us::gov::crypto::ripemd160::value_type;
        using blob_writer_t = us::gov::io::blob_writer_t;

        virtual ~olog_t();

        void init_olog(const string& home, const string& filename);
        void show_data(ostream&) const;
        inline bool need_init() const { return logos == nullptr; }
        static string ts(uint64_t);
        static string ts();
        void show_log(const hash_t& id, ostream&) const;

    public:
        ofstream* logos{nullptr};
        mutable mutex mx_logos;
        string ologfile;
    };

}

