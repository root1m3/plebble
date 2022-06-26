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
#include "db_t.h"
#include <us/gov/config.h>
#include <iomanip>
#include "node_t.h"

#define loglevel "test/sim"
#define logclass "db_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::sim;
using c = us::sim::db_t;

c::db_t() {
    nodes.reserve(32);
}

c::db_t(const db_t& other): b(other), nodes(other.nodes) {
    
}

void c::dump(const string& pfx, ostream& os) const {
    int n = 0;
    auto lpfx = pfx + "    ";
    for (auto& i: nodes) {
        os << pfx << "#" << setfill('0') << setw(2) << n++ << ": ";
        i->dump_1liner(os);
        i->dump_links(lpfx, os);
        //os << lpfx << "...\n";
        //break;

    }
}

c* c::split(int bit) {
    db_t* db0 = this;
    db_t* db1 = new db_t();
    { //accounts
        iterator i = begin();
        while (i != end()) {
            if ((*reinterpret_cast<const uint64_t*>(i->first.data()) & (1 << bit)) == 0) {
                ++i;
                continue;
            }
            db1->emplace(*i);
            i = erase(i);
        }  
    }
    { //nodes
        vector<node_t*> v0, v1;
        v0.reserve(32);
        v1.reserve(32);
        for (auto& i: nodes) {
            if ((i->id & (1 << bit)) == 0) {
                v0.emplace_back(i);
            }
            else {
                v1.emplace_back(i);
            }
        }
        db0->nodes = move(v0);
        db1->nodes = move(v1);
    }
    return db1;
}

set<node_t*> c::random_nodes(int count) {
    if (count > nodes.size()) count = nodes.size();
    set<node_t*> n;
    uniform_int_distribution<mt19937::result_type> dist(0, nodes.size() - 1);
    while (n.size() < count) {
        auto b = dist(rng);
        n.emplace(nodes[b]);
    }
    return move(n);
}

node_t* c::random_node() {
    assert(!nodes.empty());
    uniform_int_distribution<mt19937::result_type> dist(0, nodes.size() - 1);
    auto b = dist(rng);
    return nodes[b];
}

