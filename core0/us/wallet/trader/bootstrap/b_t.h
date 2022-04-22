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
#include <string>
#include "handshake_t.h"
#include "a_t.h"
#include <us/wallet/trader/kv.h>
#include <us/wallet/trader/business.h>
#include <us/wallet/trader/protocol_selection_t.h>

namespace us { namespace wallet { namespace trader { namespace bootstrap {

    using namespace std;

    struct b1_t: handshake_t {
        using params_t = trader::kv;

        b1_t();
        b1_t(const protocol_selection_t&, const params_t&);

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        protocol_selection_t protocol_selection;
        params_t params;
    };

    struct b2_t: handshake_t {
        using params_t = trader::kv;
        using personality_proof_t = trader::personality::proof_t;

        b2_t();
        b2_t(personality_proof_t&&);
        b2_t(personality_proof_t&&, params_t&&);

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        params_t params;
        personality_proof_t personality_proof;
    };

    struct b3_t: handshake_t {
        using params_t = trader::kv;

        b3_t();
        b3_t(params_t&&);

    public: //serialization
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        params_t params;
    };

}}}}

