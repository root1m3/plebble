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
//#include <unordered_set>
#include <array>
#include <cstdint>
//#include <mutex>

//#include <us/gov/types.h>
//#include <us/gov/crypto/types.h>
//#include <us/gov/socket/types.h>
#include "grid_t.h"

namespace us::gov::peer {

    struct peer_t;

    //static constexpr int clique_dimensions{1};
    //static constexpr uint8_t clique_dimension_mask{};
    
    struct clique_t: vector<grid_t*> {

        clique_t();

        void resize(uint8_t dimensions, uint8_t edges);

        peer::peer_t* pick_one();
        bool add(peer_t&, bool check_unique);
        void dump(ostream&) const;
        void watch(ostream&) const;
        vector<hostport_t> list() const;
        bool ended(peer_t*);
        void test_connectivity();

        uint8_t dimension_mask{0b00000000};

    };

}

