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

#include <string>
#include "types.h"
#include "readable.h"

namespace us::gov::io {

    struct blob_writer_t;

    struct writable {
        using version_t = readable::version_t;
        using serid_t = readable::serid_t;
        using blob_header_t = readable::blob_header_t;

        constexpr static size_t header_size() { return sizeof(version_t) + sizeof(serid_t); }

    public:
        virtual ~writable() {}

    public:
        virtual size_t blob_size() const = 0;
        virtual void to_blob(blob_writer_t&) const = 0;
        virtual serid_t serial_id() const { return 0; }

    public:
        size_t tlv_size() const;
        void write(blob_t&) const;
        [[nodiscard]] datagram* get_datagram(channel_t, svc_t svc, seq_t seq) const;
        void write(string& encoded) const;
        string encode() const;
        hash_t nft() const;
        ko save(const string& filename) const;
    };

}

