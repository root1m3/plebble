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
#include <map>
#include <set>

#include <us/gov/socket/types.h>

namespace us::gov::engine {

    struct evidence;
    struct peer_t;

    struct dbsync {
 
        ///Exchange of IPv4 addresses and evidences
        struct seeds_t: set<hostport_t> {

            void add_sample(seeds_t&& sample);
            seeds_t get_sample() const;
            
        };

        struct evidences_t: map<ts_t, evidence*> {
        };

        void process(peer_t* peer);

        evidences_t evidences;
        seeds_t seeds;
    };

}
