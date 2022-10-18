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

#include "us/gov/io/seriable.h"

#include <us/wallet/trader/params_t.h>
#include <us/wallet/trader/trader_protocol.h>
#include <us/wallet/trader/ch_t.h>

#include "dialogue_a_t.h"
#include "dialogue_b_t.h"
#include "dialogue_c_t.h"
#include "handshake_t.h"

namespace us::wallet::engine {
    struct peer_t;
}

namespace us::wallet::trader {
    struct trader_t;
}

namespace us::wallet::trader::bootstrap {
    using namespace std;

    struct bootstrapper_t {
        using trader_t = trader::trader_t;
        using params_t = trader::params_t;
        using challenge_t = a1_t::challenge_t;
        using personality_proof_t = trader::personality::proof_t;
        using ch_t = trader::ch_t;
        using peer_t = engine::peer_t;
        using blob_reader_t = us::gov::io::blob_reader_t;

        bootstrapper_t();
        virtual ~bootstrapper_t();

        virtual pair<ko, hash_t> start(trader_t&);
        virtual bool initiator() const = 0;
        virtual void offline();
        virtual void online(peer_t&);
        ko trading_msg(peer_t& peer, uint16_t svc, blob_t&&);
        virtual ko on_a(int) = 0;
        virtual ko on_c(int) = 0;

        static pair<ko, string> extract_wloc(uint16_t svc, const blob_t&);
    
    public:
        dialogue_a_t dialogue_a;
        dialogue_b_t dialogue_b;
        dialogue_c_t dialogue_c;

        hash_t trade_id;
        peer_t* peer{nullptr};
        trader_t* trader{nullptr};
    };

}

