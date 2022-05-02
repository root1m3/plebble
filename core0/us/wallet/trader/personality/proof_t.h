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
#include <iostream>

#include <us/gov/engine/signed_data.h>
#include <us/gov/crypto/ec.h>
#include <us/gov/io/seriable.h>
#include <us/gov/crypto/ripemd160.h>

namespace us { namespace wallet { namespace trader {  namespace personality {

    struct raw_personality_t: virtual us::gov::io::seriable {
        using hash_t = us::gov::crypto::ripemd160::value_type;
        using moniker_t = string;

        raw_personality_t();
        raw_personality_t(const raw_personality_t&);
        raw_personality_t(const hash_t&, const moniker_t&);
        raw_personality_t(hash_t&&, moniker_t&&);

        bool reset(raw_personality_t&&);
        void dump(ostream&) const;
        void dump(const string& pfx, ostream&) const;
        string to_string() const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        hash_t id;
        moniker_t moniker; //not included in signature
    };

    struct proof_t: us::gov::engine::signed_data0, raw_personality_t {
        using b = us::gov::engine::signed_data0;
        using b2 = raw_personality_t;
        using pub_t = us::gov::crypto::ec::keys::pub_t;
        using raw_t = raw_personality_t;
        using challenge_t = sigmsg_hash_t;

        raw_t raw() const;
        bool verify(const challenge_t&, ostream&) const;
        bool verify(const challenge_t&) const;

        static sigmsg_hash_t msg(const hash_t& id, const challenge_t&);

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    };

}}}}

