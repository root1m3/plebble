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
#include "peer_t.h"
#include "clique_t.h"

#define loglevel "gov/peer"
#define logclass "clique_t"
#include "logs.inc"

using namespace std;
using namespace us::gov::peer;
using c = us::gov::peer::clique_t;

c::clique_t() {
}

peer::peer_t* c::pick_one() {
    static mt19937_64 rng(random_device{}());
    uniform_int_distribution<> d(0, size() - 1);
    auto i = begin();
    advance(i, d(rng));
    return (*i)->pick_one();
}

bool c::add(peer_t& peer, bool check_unique) {
    auto d = peer.pubkey.lsb() & dimension_mask;
    auto i = begin();
    advance(i, d);
    return (*i)->add(peer, check_unique);
}

void c::dump(ostream& os) const {
    int d = 0;
    for (auto i: *this) {
        os << "clique dimension " << d++ << '/' << size() << ":\n";
        i->dump(os);
    }
}

void c::watch(ostream& os) const {
    int d = 0;
    for (auto i: *this) {
        os << "clique dimension " << d++ << '/' << size() << ":\n";
        i->dump(os);
    }
}

void c::resize(uint8_t dimensions, uint8_t edges) {
    assert(empty());
    assert(dimensions > 0);
    assert((dimensions & (dimensions - 1)) == 0); //check dimensions is a power of 2 (so id-mask works)
    reserve(dimensions);
    for (uint8_t i = 0; i < dimensions; ++i) {
        emplace_back(new grid_t())->resize(edges, nullptr);
    }
    dimension_mask = 1 - dimensions;
}

vector<hostport_t> c::list() const {
    vector<hostport_t> v;
    for (auto& pgrid: *this) {
        auto& grid = *pgrid;
        unique_lock<mutex> lock(grid.mx_);
        for (auto i: grid) {
            if (i != nullptr) {
                v.emplace_back(i->hostport);
            }
        }
    }
    return move(v);
}

bool c::ended(peer_t* p) {
    for (auto grid: *this) {
        if (grid->ended(p)) return true;
    }
    return false;
}

void c::test_connectivity() {
    log("test connectivity");
    for (auto& grid: *this) {
        lock_guard<mutex> lock(grid->mx_);
        for (auto i = grid->begin(); i != grid->end(); ++i) {
            if ((*i) != nullptr) (*i)->ping();
        }
    }
}

