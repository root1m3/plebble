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
#include <map>
#include <iostream>
#include <inttypes.h>

#include <us/gov/types.h>
#include <us/gov/io/seriable.h>
#include <us/gov/crypto/ec.h>

#include "app.h"
#include "signed_data.h"
#include "types.h"

namespace us::gov::engine {

    struct local_deltas_t final: map<appid_t, app::local_delta*>, signed_data, virtual io::seriable {
        using b = map<appid_t, app::local_delta*>;
        using s = io::seriable;

        local_deltas_t(): id(0) {}
        local_deltas_t(const local_deltas_t&) = delete;
        ~local_deltas_t();

    public:
        void clear();

        using s::get_datagram;
        datagram* get_datagram(channel_t, seq_t) const;

    public:
        void hash_data_to_sign(sigmsg_hasher_t&) const override;
        void hash_data(hasher_t&) const override;
        void dumpX(ostream&) const;

    public:
        using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'L'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        ts_t id;
    };

}

namespace us { namespace gov { namespace io {
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::gov::engine::local_deltas_t& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::gov::engine::local_deltas_t& o) { return o.get_datagram(channel, svc, seq); }
}}}

