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
#include "a_t.h"
#include <mutex>
#include <us/wallet/trader/endpoint_t.h>
#include "protocols_t.h"

namespace us::wallet::engine {
    struct peer_t;
}

namespace us::wallet::trader {
    struct trader_t;
}

namespace us::wallet::trader::bootstrap {
    using namespace std;

    struct bootstrapper_t;

    struct dialogue_a_t {
        using personality_proof_t = trader::personality::proof_t;
        using endpoint_t = trader::endpoint_t;
        using peer_t = engine::peer_t;

        static constexpr auto KO_43342 = "KO_43342 Out of sequence.";

        dialogue_a_t(bootstrapper_t& parent);
        ~dialogue_a_t();

        struct state_t {
            int seq{0};
        };

        ko initiate(peer_t&, const string& wloc);
        ko handshake(peer_t&, a1_t&&);
        ko handshake(peer_t&, a2_t&&);
        ko handshake(peer_t&, a3_t&&);
        ko update_peer(peer_t&, ch_t&&);
        ko update_peer2(peer_t&, ch_t&&);
        void reset();
        state_t* release_();

    public:
        state_t* state{nullptr};
        mutex mx;
        bootstrapper_t& parent;
    };

}

