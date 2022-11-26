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
#include <cassert>
#include <string>
#include <functional>
//#include <utility>
#include <type_traits>

#include <us/gov/config.h>
//#include <us/gov/likely.h>
#include <us/gov/types.h>
#include <us/gov/crypto/types.h>
#include <us/gov/socket/types.h>

#include "factory.h"
#include "types.h"

namespace us::gov::io {

    struct blob_reader_t;

    struct readable {
        using version_t = uint8_t;
        using serid_t = uint8_t;

        struct blob_header_t {
            version_t version;
            serid_t serid;
        };

        virtual ~readable() {}

    public:
        virtual ko from_blob(blob_reader_t&) = 0;
        virtual serid_t serial_id() const { return 0; }

    public:
        ko read(const blob_t&);
        ko read(const datagram&);
        ko read(const string& blob_b58);
        ko load(const string& filename);
        pair<ko, blob_header_t> read1(const blob_t&);
        pair<ko, blob_header_t> read1(const string& blob_b58);
        pair<ko, pair<blob_header_t, hash_t>> read2(const blob_t&);
        pair<ko, pair<blob_header_t, hash_t>> read2(const string& blob_b58);
        pair<ko, blob_header_t> load1(const string& filename);
        pair<ko, pair<blob_header_t, hash_t>> load2(const string& filename);
        pair<ko, blob_t> load3(const string& filename);
        [[nodiscard]] static pair<ko, readable*> load(const string& filename, function<readable*(const serid_t&)>);

    };

}

