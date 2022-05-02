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

#include "b_t.h"
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/protocol_selection_t.h>
#include <mutex>

namespace us { namespace wallet { namespace engine {
    struct peer_t;
}}}

namespace us { namespace wallet { namespace trader {
    struct trader_t;
}}}

namespace us { namespace wallet { namespace trader { namespace bootstrap {

    using namespace std;

    struct bootstrapper_t;

    struct dialogue_b_t {
        using ch_t = trader::ch_t;
        using peer_t = engine::peer_t;

        static constexpr auto KO_43343 = "KO_43343 Out of sequence.";

        struct state_t {
            int seq{0};
        };

        dialogue_b_t(bootstrapper_t& parent);
        ~dialogue_b_t();

        ko initiate(peer_t&, protocol_selection_t&&, ch_t&);
        ko handshake(peer_t&, b1_t&&);
        ko handshake(peer_t&, b2_t&&);
        ko handshake(peer_t&, b3_t&&);
        ko update_peer(peer_t&, ch_t&& ch, bool b = false);
        void reset();
        state_t* release_();

        state_t* state{nullptr};
        mutex mx;
        bootstrapper_t& parent;
    };

}}}}

