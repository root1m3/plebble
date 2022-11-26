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
#include "qr_t.h"

#include <map>
#include <string>
#include <iostream>

#include <us/gov/crypto/ripemd160.h>
#include <us/gov/io/seriable.h>

namespace us::wallet::trader {

    using namespace us::gov;
    struct traders_t;

    struct bookmark_info_t: virtual gov::io::seriable {

        bookmark_info_t();
        bookmark_info_t(const bookmark_info_t&);
        bookmark_info_t(bookmark_info_t&&);
        bookmark_info_t(string&& label, blob_t&& ico);
        bookmark_info_t& operator = (const bookmark_info_t&);
        bookmark_info_t& operator = (bookmark_info_t&&);
        void dump(const string& pfx, ostream& os) const;

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string label;
        blob_t ico;
    };

    struct bookmark_t: bookmark_info_t {
        using endpoint_t = us::wallet::trader::endpoint_t;
        using b = bookmark_info_t;

        bookmark_t();
        bookmark_t(qr_t&&, bookmark_info_t&&);
        bookmark_t(const qr_t&, bookmark_info_t&&);
        bookmark_t(const bookmark_t&);
        bookmark_t& operator = (const bookmark_t&);
        bookmark_t(string&& ep, string&& protocol, const string&& role, string&& label, blob_t&& ico);

        void dump(const string& pfx, ostream& os) const;

    public: //serialization
        //using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'b'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        qr_t qr;
    };

    struct bookmarks_t: map<string, bookmark_t>, virtual gov::io::seriable  {

        bookmarks_t& operator += (const bookmarks_t&);
        ko add(string name, bookmark_t&&);
        ko add(string name, const bookmark_t&);
        ko add(const bookmarks_t&);
        ko remove(const string& key);
        ko name_check(string& name) const;
        void dump(const string& pfx, ostream& os) const;
        const_iterator find_protocol_role(const string& prot, const string& role) const;

    public: //serialization
        //using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'B'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;
    };


}
/*
namespace us::gov::io {

    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::wallet::trader::bookmark_t& o) { return o.read(d); }
    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::wallet::trader::bookmarks_t& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::wallet::trader::bookmark_t& o) { return o.get_datagram(channel, svc, seq); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::wallet::trader::bookmarks_t& o) { return o.get_datagram(channel, svc, seq); }

}
*/


