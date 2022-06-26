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

#include <set>
//#include <map>
#include "sim.h"
#include "maskcoord_t.h"

namespace us::sim {

    using namespace std;

    struct node_t;

    struct tx_t {
        tx_t(int amount);

        address_t from;
        address_t to;
        int amount;
        uint64_t id;

        static size_t next_id;
    };

    struct rtx_t {

       // using distance_t = uint32_t;

        rtx_t(const tx_t* tx, size_t sersize);
        rtx_t(const tx_t* tx, size_t sersize, uint32_t coord);
        rtx_t(const rtx_t&);
        rtx_t(const rtx_t&, set<uint32_t>&&);
        ~rtx_t();

        bool hit_target_shard(const maskcoord_t& key);
        void dump_route(const maskcoord_t& key, ostream& os) const;
        //void dump_route(ostream& os) const;
/*
        inline rtx_t& setd(uint32_t v) {
            d = v;
            return *this;
        }
*/
        //distance_t distance_to(const maskcoord_t&) const;

        int ttl;
        size_t sersize;
        const tx_t* tx;
        set<uint32_t> route;
        //set<node_t*> visited;
//        uint32_t d{numeric_limits<uint32_t>::max()};
    };

}

