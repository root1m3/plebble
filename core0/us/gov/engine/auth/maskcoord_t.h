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

#include <cstdint>
#include <iostream>

namespace us::gov::engine::auth {

    using namespace std;

    struct maskcoord_t {
        using mask_t = uint32_t;
        using coord_t = uint32_t;

        inline maskcoord_t(mask_t m, coord_t c): mask(m), coord(c) {}
        inline maskcoord_t(): mask(0), coord(0) {}
       
        bool operator == (const maskcoord_t& other) const;
        inline bool operator < (const maskcoord_t& other) const {
            if (mask == other.mask) return coord < other.coord;
            return mask < other.mask;
        }

        void dump_1liner(ostream& os) const;

        void clear();

    public:
        mask_t mask;
        coord_t coord;
        bool mode{0}; //0-maskbit 1-maskcoord
    };

}

std::ostream& operator << (std::ostream&, const us::gov::engine::auth::maskcoord_t&);

