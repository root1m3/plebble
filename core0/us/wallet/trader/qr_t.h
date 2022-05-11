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
#include "endpoint_t.h"
#include "protocol_selection_t.h"
#include <iostream>
#include <us/gov/io/seriable.h>

namespace us::wallet::trader {

    using namespace us::gov;

    struct qr_t: virtual gov::io::seriable {

        qr_t();
        qr_t(endpoint_t&&, protocol_selection_t&&);
        qr_t(const endpoint_t&, protocol_selection_t&&);
        qr_t(const endpoint_t&);
        qr_t(const qr_t&);
        qr_t& operator = (const qr_t&);
        void dump(const string& pfx, ostream& os) const;

    public: //serialization blob
        using serid_t = blob_reader_t::serid_t;
        static constexpr serid_t serid{'Q'};
        serid_t serial_id() const override { return serid; }
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public: //serialization string
        ko from(const string&);
        ko from_streamX(istream&);
        void to_streamX(ostream&) const;
        string to_string() const;

    public:
        endpoint_t endpoint;
        protocol_selection_t protocol_selection;

    };

}

namespace us::gov::io {

    template<> inline ko blob_reader_t::readD(const us::gov::socket::datagram& d, us::wallet::trader::qr_t& o) { return o.read(d); }
    template<> inline datagram* blob_writer_t::get_datagram(channel_t channel, svc_t svc, seq_t seq, const us::wallet::trader::qr_t& o) { return o.get_datagram(channel, svc, seq); }

}

