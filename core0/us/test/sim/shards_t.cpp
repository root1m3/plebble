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
#include "shards_t.h"

#include <bitset>

#include <us/gov/config.h>

#include "db_t.h"
#include "node_t.h"

#define loglevel "test/sim"
#define logclass "shards_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::sim;
using c = us::sim::shards_t;

c::shards_t(): db(new db_t()), shard0(nullptr), shard1(nullptr), parent(nullptr) {
    
}

c::shards_t(shards_t* parent): db(nullptr), shard0(nullptr), shard1(nullptr), parent(parent) {
    
}

c::~shards_t() {
    delete db;
    delete shard0;
    delete shard1;
}

c::stats_t c::stats() const {
    stats_t st;
    stats(st);
    return move(st);
}

node_t* c::random_node() {
    static uniform_int_distribution<mt19937::result_type> sdist(0, 1);
    if (db != nullptr) {
        uniform_int_distribution<mt19937::result_type> dist(0, db->nodes.size() - 1); // distribution in range [0, N)
        auto b = dist(rng);
        return db->nodes[b];
    }
    if (shard0 != nullptr) {
        auto b = sdist(rng);
        if (b == 0) {
            return shard0->random_node();
        }
        else {
            return shard1->random_node();
        }
    }
    return nullptr;
}

void c::apply(const function<void(node_t&)>& f) const {
    if (db != nullptr) {
        for (auto& i: db->nodes) {
            f(*i);
        }
    }
    if (shard0 != nullptr) {
        shard0->apply(f);
        shard1->apply(f);
    }
}

void c::apply(const function<void(node_t&, shards_t&)>& f) {
    if (db != nullptr) {
        for (auto& i: db->nodes) {
            f(*i, *this);
        }
    }
    if (shard0 != nullptr) {
        shard0->apply(f);
        shard1->apply(f);
    }
}

void c::apply(const function<void(db_t&)>& f) {
    if (db != nullptr) {
        f(*db);
        return;
    }
    if (shard0 != nullptr) {
        shard0->apply(f);
        shard1->apply(f);
    }
}

void c::dump0(ostream& os) const {
    auto i = this;
    while (i->parent != nullptr) {
        i = i->parent;
    }
    i->dump(os);
}

void c::dump(ostream& os) const {
    int n{0};
    dump("  ", n, os);
}

void c::dump_1liner(ostream& os) const {
    os << "shard key " << key << endl;
}

void c::dump(const string& pfx, int& n, ostream& os) const {
    if (db != nullptr) {
        os << pfx << "db " << n++ << ": mask-coord ";
        key.dump_1liner(os);
        os << '\n';
        db->dump(pfx + "    ", os);
        return;
    }
    if (shard0 == nullptr) return;
    assert(shard1 != nullptr);
    os << pfx << "-> bit " << bitset<32>(key.mask) << ": 0\n";
    shard0->dump(pfx + "    ", n, os);
    os << pfx << "-> bit " << bitset<32>(key.mask) << ": 1\n";
    shard1->dump(pfx + "    ", n, os);
}

bool c::emplace(node_t* n) {
    if (!emplace0(n)) return false;
    if (db == nullptr) return true;
    if (db->nodes.size() < 32) return true;
    assert(shard0 == nullptr);
    assert(shard1 == nullptr);
    int highestbit = 0;
    {
        auto v = key.mask;
        while (true) {
            if (v == 0) break;
            v >>= 1;
            ++highestbit;
        }
    }
    using mask_t = maskcoord_t::mask_t;

    mask_t bit = (1 << highestbit);
    auto newm = key.mask | bit;

    auto db0 = db;
    auto db1 = db0->split(highestbit);

    shard0 = new shards_t();
    shard0->parent = this;
    shard0->key = maskcoord_t(newm, key.coord);
    shard0->db = db0;

    shard1 = new shards_t();
    shard1->parent = this;
    shard1->key = maskcoord_t(newm, key.coord | bit);
    shard1->db = db1;

    db = nullptr;
    key.mask = bit;
    key.mode = 1;
    return true;
}


bool c::emplace0(node_t* n) {
    assert(n != nullptr);
    if (shard0 != nullptr) {
        if ((n->id & key.mask) == 0) {
            if (shard0->emplace(n)) return true;
        }
        else {
            if (shard1->emplace(n)) return true;
        }
    }
    else {
        if ((key.mask & n->id) == key.coord) {
            db->nodes.emplace_back(n);
            return true;
        } 
    }
    return false;
}

int c::depth() const {
    int d = 0;
    auto s = parent;
    while (s != nullptr) {
        s = s->parent;
        ++d;
    }
    return d;    
}

c* c::root() {
    auto s = this;
    while (s->parent != nullptr) {
        s = s->parent;
    }
    return s;    
}

void c::stats_t::dump(ostream& os) const {
    os << "stats: \n";
    os << "nodes " << nodes << '\n';
    os << "accounts " << accounts << '\n';
    os << "shards " << shards << '\n';
    os << "edges_consensus " << edges_consensus << '\n';
    os << "edges_network " << edges_network << '\n';
    os << "transmissions " << transmissions << '\n';
    os << "txdata_in " << txdata_in << '\n';
    os << "txdata_out " << txdata_out << '\n';

    os << "tx_arrived_tot " << tx_arrived_tot << '\n';
    os << "tx_arrived_from_same_shard " << tx_arrived_from_same_shard << '\n';
    os << "tx_arrived_from_other_shard " << tx_arrived_from_other_shard << '\n';
    os << "tx_relayed_consensus " << tx_relayed_consensus << '\n';
    os << "tx_relayed_network " << tx_relayed_network << '\n';

/*
    nodes_per_shard.dump("nodes_per_shard ", os);
    accounts_per_shard.dump("accounts_per_shard ", os);

    transmissions_per_node.dump("transmissions_per_node ", os);
    txdata_in_per_node.dump("txdata_in_per_node ", os);
    txdata_out_per_node.dump("txdata_out_per_node ", os);
    edges_consensus_per_node.dump("edges_consensus_per_node ", os);
    edges_network_per_node.dump("edges_network_per_node ", os);
*/
}

void c::stats(stats_t& st) const {
    if (db != nullptr) {
        ++st.shards;
        st.nodes += db->nodes.size();
        st.accounts += db->size();
        for (auto& i: db->nodes) {
            st.edges_consensus += i->links_consensus.size();
            st.edges_network += i->links_network.size();

/*
            st.tx_arrived_tot += i->tx_arrived_tot;
            st.tx_arrived_from_same_shard += i->tx_arrived_from_same_shard;
            st.tx_arrived_from_other_shard += i->tx_arrived_from_other_shard;
            st.tx_relayed_consensus += i->tx_relayed_consensus;
            st.tx_relayed_network += i->tx_relayed_network;
*/
        }
    }
    if (shard0 != nullptr) {
        assert(shard1 != nullptr);
        shard0->stats(st);
        shard1->stats(st);
    }
}
c::stats_t::stdata::stdata(const string& name): name(name), num(0), acc(0), min(numeric_limits<size_t>::max()), max(0) {
}

c::stats_t::stdata& c::stats_t::stdata::operator += (size_t value) {
    ++num;
    acc += value;
    if (value < min) min = value;
    else if (value > max) max = value;
    return *this;
}

void c::stats_t::stdata::dump(const string& pfx, ostream& os) const {
    os << pfx << name << " count " << num << " min " << min << " avg " << avg << " max " << max;
}

ostream& operator << (ostream& os, const shards_t::stats_t::stdata& o) {
    o.dump("", os);
    return os;
}

