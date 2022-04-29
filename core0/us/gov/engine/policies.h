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
#include <array>

namespace us::gov::engine {

    using namespace std;

    struct param_t {
        enum consensus_t { majority = 0, average, median, num_types };
        constexpr static array<const char*, num_types> typestr = {"majority", "average", "median"};

        param_t(int id, consensus_t type): id(id), consensus_type(type) {}
        inline bool operator <(const param_t& other) const { return id < other.id; }

        int id;
        consensus_t consensus_type;
    };

}

