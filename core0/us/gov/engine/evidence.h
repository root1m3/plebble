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
#include <chrono>
#include <string>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/io/seriable.h>
#include <us/gov/types.h>

namespace us::gov::socket {
    struct datagram;
}

namespace us::gov::engine {

    struct evidence: virtual io::seriable {
        using s = io::seriable;

        static constexpr auto tx_begin_en = "---transaction---------------";
        static constexpr auto tx_end_en = "-/-transaction---------------";
        static constexpr auto tx_begin_es = "---transaccion---------------";
        static constexpr auto tx_end_es = "-/-transaccion---------------";

        evidence(appid_t app, eid_t eid);
        evidence(const evidence&);
        virtual ~evidence();

        static string formatts(ts_t ts);
        virtual string name() const = 0;
        virtual void hash_data_to_sign(sigmsg_hasher_t&) const;
        virtual void hash_data(hasher_t&) const;
        hash_t hash_id() const;
        virtual bool verify(ostream&) const;
        void write_pretty(const string& lang, ostream&) const;
        virtual void write_pretty_en(ostream&) const;
        virtual void write_pretty_es(ostream&) const;
        void update_ts(const ts_t& nsecs_in_future = 0);
        void write_sigmsg(sigmsg_hasher_t&, bool include_ts) const;
        static pair<ko, pair<appid_t, eid_t>> extract_instance_idX(io::blob_reader_t&);
        static pair<ko, evidence*> create(appid_t, eid_t);
        static pair<ko, evidence*> from_blob(const blob_t&);
        static pair<ko, evidence*> from_b58(const string& blob_b58);
        virtual bool check_amounts() const { return true; }
        using s::get_datagram;
        datagram* get_datagram(channel_t, seq_t) const;

    public:
        using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'E'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        ts_t ts;
        appid_t app;
        eid_t eid;
    };

}

namespace us::gov::io {
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::gov::engine::evidence& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::gov::engine::evidence& o) { return o.get_datagram(channel, svc, seq); }
}

