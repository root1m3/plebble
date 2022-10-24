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
#include <us/gov/io/seriable.h>
#include <us/gov/socket/types.h>

namespace us::wallet::engine {

    struct ip4_endpoint_t final: gov::io::seriable {

        ip4_endpoint_t();
        ip4_endpoint_t(shost_t, port_t, channel_t);
        ip4_endpoint_t(host_t, port_t, channel_t);
        ip4_endpoint_t(const ip4_endpoint_t&);

    public:
        bool operator == (const ip4_endpoint_t&) const;
        string to_string() const;
        shostport_t shostport() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        shost_t shost;
        port_t port;
        channel_t channel;
    };

}

