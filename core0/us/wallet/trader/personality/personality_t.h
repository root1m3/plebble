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
#include "proof_t.h"
#include <string>
#include <us/gov/crypto/ec.h>
#include <us/gov/crypto/ripemd160.h>

namespace us::wallet::trader::personality {

    using namespace std;

    struct personality_t {
        using keys = us::gov::crypto::ec::keys;
        using priv_t = keys::priv_t;
        using pub_t = keys::pub_t;
        using hash_t = us::gov::crypto::ripemd160::value_type;
        using sigmsg_hasher_t = us::gov::engine::signed_data::sigmsg_hasher_t;
        using sigmsg_hash_t = sigmsg_hasher_t::value_type;
        using challenge_t = proof_t::challenge_t;
        using moniker_t = string;

        static constexpr uint8_t version{0};

        personality_t();
        personality_t(const personality_t&);
        personality_t(const priv_t&, const moniker_t&);

        personality_t& operator = (const personality_t&);

        static challenge_t gen_challenge();
        static personality_t generate(const moniker_t&);

        void reset(const priv_t& sk, const moniker_t&);
        bool is_anonymous() const { return !k.pub.valid; }
        priv_t k_sign_docs() const;
        int reset_if_distinct(const priv_t&, const moniker_t&);
        int reset_if_distinct(const string& ssk, const moniker_t&);
        string one_liner() const;
        void one_liner(ostream&) const;
        int set_anonymous_only_key(); //moniker remains
        int set_anonymous();
        inline const priv_t& private_key() const { return k.priv; }
        inline const pub_t& public_key() const { return k.pub; }
        proof_t gen_proof(const challenge_t&) const;

    public:
        moniker_t moniker;
        hash_t id;
        keys k;
    };

}

