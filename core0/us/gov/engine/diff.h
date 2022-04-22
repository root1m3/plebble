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
#include <vector>
#include <cassert>
#include <unordered_map>
#include <thread>
#include <map>
#include <set>

#include <us/gov/types.h>
#include <us/gov/io/seriable.h>
#include <us/gov/io/seriable_map.h>
#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/types.h>
#include <us/gov/crypto/ec.h>

#include "app.h"
#include "signed_data.h"

namespace us { namespace gov { namespace engine {

    struct peer_t;
    struct diff;
    struct local_deltas_t;

    struct pow_t final: io::seriable_map<hash_t, uint64_t> {
        uint64_t sum() const;
        void dump(const string& prefix, ostream&) const;
    };

    struct diff final: map<appid_t, app::delta*>, virtual seriable {
        using b = map<appid_t, app::delta*>;

    public:
        diff() {}
        diff(const diff&) = delete;
        ~diff();
        void clear();

    public:
        uint64_t add(int appid, app::local_delta*);
        bool add(local_deltas_t*);
        void close();
        const hash_t& hash() const;
        const hash_t& hash(const blob_t&) const; /// assumes blob passed in matches the blob from write
        void dump(const string& prefix, ostream&) const;

    public:
        using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'D'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        mutable hash_t hash_cached;
        mutable bool h{false};
        hash_t prev;
        uint64_t id;
        mutex mx;
        pow_t proof_of_work;
        mutex mx_proof_of_work;
    };

    using base = diff;

}}}

namespace us { namespace gov { namespace io {
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::gov::engine::diff& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::gov::engine::diff& o) { return o.get_datagram(channel, svc, seq); }
}}}

