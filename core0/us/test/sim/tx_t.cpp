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
#include "tx_t.h"
#include <bitset>
#include <us/gov/config.h>
#include "maskcoord_t.h"

#define loglevel "test/sim"
#define logclass "tx_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::sim;
using c = us::sim::tx_t;
using r = us::sim::rtx_t;

uint64_t c::next_id{6758392};

c::tx_t(int amount): id(next_id++), amount(amount) {
    from.fill_random(rng);
    to.fill_random(rng);
}

r::rtx_t(const tx_t* tx, size_t sersize): tx(tx), sersize(sersize), ttl(10) {
    route.emplace(*reinterpret_cast<const uint32_t*>(tx->from.data()));
    route.emplace(*reinterpret_cast<const uint32_t*>(tx->to.data()));
    cout << "transfer from " << tx->from << "(" << bitset<32>(*route.begin()) << ") to " << tx->to << "(" << bitset<32>(*route.rbegin()) << ")" << endl;
}

r::rtx_t(const tx_t* tx, size_t sersize, uint32_t coord): tx(tx), sersize(sersize), ttl(10) {
    route.emplace(coord);
}

r::rtx_t(const rtx_t& other): sersize(other.sersize), tx(other.tx), route(other.route), ttl(other.ttl) {
}

r::rtx_t(const rtx_t& other, set<uint32_t>&& route): sersize(other.sersize), tx(other.tx), route(route), ttl(other.ttl) {
}

r::~rtx_t() {
}

bool r::hit_target_shard(const maskcoord_t& key) {
    if (route.empty()) return true;
    bool b = false;
    auto i = route.begin();
    while (i != route.end()) {
        if ((*i & key.mask) == key.coord) {
            i = route.erase(i);
            b = true;
            continue;
        }
        ++i;
    }
    return b;
}

void r::dump_route(const maskcoord_t& key, ostream& os) const {
    os << "rtx.route from maskcoord " << key << "\n";
    for (auto& i: route) {
        os << "    content " << bitset<32>(i) << '\n';
    }
    os << '\n';
}
/*
void r::dump_route(ostream& os) const {
    for (auto& i: route) {
        os << bitset<32>(i);
    }
    os << '\n';
}
*/
/*
r::distance_t r::distance_to(const maskcoord_t& mc) const {
    distance_t mind = numeric_limits<distance_t>::max();
    for (auto& r: route) {
        auto d0 = (mc.mask & r) ^ mc.coord;
        if (d0 < mind) {
            mind = d0;
        }
    }
    return mind;
}

*/

