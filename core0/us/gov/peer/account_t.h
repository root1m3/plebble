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
#include <us/gov/types.h>
#include <us/gov/io/seriable.h>

namespace us { namespace gov { namespace peer {

    struct account_t: virtual io::seriable {

        account_t() {}
        account_t(host_t net_address, port_t port, uint16_t seen, uint16_t w);
        account_t(const account_t&);

        account_t& operator = (int zero);
        bool operator == (const account_t&) const;
        account_t& operator = (const account_t&);

        void dump(ostream&) const;
        void hash_data_to_sign(sigmsg_hasher_t&) const;
        void hash_data(hasher_t&) const;
        void dump_as_seeds(ostream&) const;
        ko check(channel_t) const;
        string endpoint() const;
        void print_endpoint(ostream&) const;
        void hash(crypto::ripemd160&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        host_t net_address;
        port_t port;
        uint16_t seen;
        uint16_t w;
    };

}}}

namespace std {
    template <>
    struct hash<us::gov::peer::account_t> {
        size_t operator()(const us::gov::peer::account_t&) const noexcept;
    };
}


