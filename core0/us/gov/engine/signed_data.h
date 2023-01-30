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
#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/ripemd160.h>
#include <us/gov/io/seriable.h>
#include <string>

namespace us::gov::engine {

    using blob_writer_t = us::gov::io::blob_writer_t;
    using blob_reader_t = us::gov::io::blob_reader_t;
    using seriable = us::gov::io::seriable;

    struct signed_data0: virtual seriable {
        using keys = crypto::ec::keys;
        using pubkey_t = keys::pub_t;
        using privkey_t = keys::priv_t;
        using sigmsg_hasher_t = crypto::ec::sigmsg_hasher_t;
        using sigmsg_hash_t = crypto::ec::sigmsg_hasher_t::value_type;
        using sig_t = us::gov::crypto::ec::sig_t;

    public:
        virtual ~signed_data0() {}
        void write_pretty(const string& lang, ostream&) const;

    public:
        void sign(const keys&, const sigmsg_hasher_t::value_type&);
        bool verify(const sigmsg_hasher_t::value_type&, ostream&) const;
        bool verify(const sigmsg_hasher_t::value_type&) const;

    public:
        virtual void write_pretty_en(ostream&) const;
        virtual void write_pretty_es(ostream&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        pubkey_t pubkey;
        sig_t signature;
    };

    struct signed_data: signed_data0 {
        using b = signed_data0;
        using ripemd160 = us::gov::crypto::ripemd160;

        virtual void hash_data_to_sign(sigmsg_hasher_t&) const = 0;
        virtual void hash_data(ripemd160&) const = 0;
        ripemd160::value_type hash() const;

    public:
        using b::sign;
        using b::verify;
        void sign(const keys&);
        void sign(const privkey_t&);
        virtual bool verify(ostream&) const;
        virtual bool verify() const;
        virtual void write_pretty_en(ostream&) const;
        virtual void write_pretty_es(ostream&) const;
    };

}

