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
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/io/seriable.h>
#include <us/gov/socket/types.h>

namespace us { namespace wallet { namespace trader {

    using namespace us::gov;
    struct traders_t;

    struct endpoint_t: virtual gov::io::seriable {
        endpoint_t();
        endpoint_t(const string&);
        endpoint_t(const hash_t& pkh, const string& subhome);
        endpoint_t(const endpoint_t&);

        endpoint_t& operator = (const endpoint_t&);
        bool operator == (const endpoint_t&) const;
        bool operator != (const endpoint_t&) const;
        string filestr() const;
        void hash(us::gov::crypto::ripemd160& hasher) const;
        hostport_t decode_ip4() const;
        string decode_ip4_string() const;
        static hostport_t decode_ip4(const hash_t&);
        static string decode_ip4_string(const hash_t&);
        static hash_t encode_ip4(host_t ip4, port_t port);
        static hash_t encode_ip4(const hostport_t& ipport);
        static hash_t encode_ip4(const string&);
        bool is_ip4() const;
        inline static bool is_ip4(const hash_t& h) { return decode_ip4(h).first != 0; }
        void dump(const string& prefix, ostream&) const;
        void clear();

    public: //serialization string
        void to_streamX(ostream&) const; //no channel
        ko from_streamX(istream&);
        string to_string() const;
        ko from_string(const string&);

    public: //serialization blob
        using serid_t = blob_reader_t::serid_t;
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        channel_t chan;
        hash_t pkh;
        string wloc;
    };

}}}

namespace us { namespace gov { namespace io {
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::wallet::trader::endpoint_t& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::wallet::trader::endpoint_t& o) { return o.get_datagram(channel, svc, seq); }
}}}

