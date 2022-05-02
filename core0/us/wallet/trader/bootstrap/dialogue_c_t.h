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

#include "c_t.h"
#include <mutex>
#include <us/wallet/trader/endpoint_t.h>
#include <us/wallet/trader/protocol_selection_t.h>
#include "protocols_t.h"

namespace us { namespace wallet { namespace engine {
    struct peer_t;
}}}

namespace us { namespace wallet { namespace trader {
    struct trader_t;
}}}

namespace us { namespace wallet { namespace trader { namespace bootstrap {

    using namespace std;

    struct bootstrapper_t;

    struct dialogue_c_t {
        using personality_proof_t = trader::personality::proof_t;
        using endpoint_t = trader::endpoint_t;
        using peer_t = engine::peer_t;

        static constexpr auto KO_43344 = "KO_43344 Out of sequence.";

        struct state_t {
            int seq{0};
            int nc2{1};
            int nc3_4{10}; //combined 3 and 4 arrivals, deadlock cut after this many back and forths
        };

        dialogue_c_t(bootstrapper_t& parent);
        ~dialogue_c_t();

        ko initiate(peer_t&, const string& wloc, const protocol_selection_t&);
        ko handshake(peer_t&, c1_t&&);
        ko handshake(peer_t&, c2_t&&);
        ko handshake(peer_t&, c3_t&&);
        ko handshake(peer_t&, c4_t&&);
        ko update_peer(peer_t&, ch_t&&);
        ko update_peer2(peer_t&, ch_t&&);
        void reset();
        state_t* release_();

    public:
        state_t* state{nullptr};
        mutex mx;
        bootstrapper_t& parent;
    };

}}}}

