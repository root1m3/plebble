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

#include "handshake_t.h"

#include <us/gov/crypto/ripemd160.h>

#include <us/wallet/trader/endpoint_t.h>
#include <us/wallet/trader/trader_protocol.h>
#include <us/wallet/trader/protocol_selection_t.h>

#include "bootstrapper_t.h"

namespace us::wallet::trader::bootstrap {

    using namespace std;

    struct initiator_t: bootstrapper_t {
        using b = bootstrapper_t;
        using endpoint_t = us::wallet::trader::endpoint_t;
        using protocol = us::wallet::trader::trader_protocol;
        using hash_t = us::gov::crypto::ripemd160::value_type;

        struct inverted_qr_t: qr_t {
            inverted_qr_t(qr_t&&);

            size_t blob_size() const override { return 0; }
            void to_blob(blob_writer_t&) const override {}
            ko from_blob(blob_reader_t&) override { return "KO 68794"; }
        };

        initiator_t(inverted_qr_t&&, wallet::local_api&);
        ~initiator_t() ;

    public:
        bool initiator() const override { return true; }
        pair<ko, hash_t> start(trader_t&) override;
        void online(peer_t&) override;
        ko on_a(int) override;
        ko on_c(int) override;
        hash_t make_new_id() const;

    public:
        handshake_t* handshake{nullptr};
        inverted_qr_t inverted_qr;
        wallet::local_api& w;
    };

}

