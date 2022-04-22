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
#include <us/gov/crypto/ripemd160.h>
#include <us/wallet/engine/peer_t.h>

#include "bootstrapper_t.h"

namespace us { namespace wallet { namespace trader { namespace bootstrap {

    struct follower_t final: bootstrapper_t {
        using b = bootstrapper_t;
        using peer_t = us::wallet::engine::peer_t;
        using hash_t = us::gov::crypto::ripemd160::value_type;

        follower_t(const hash_t& trade_id, peer_t&);
        bool initiator() const override { return false; }
        pair<ko, hash_t> start(trader_t&) override;
        void offline() override;
        ko on_a(int) override;
        ko on_c(int) override;

        peer_t& peer;
    };

}}}}

