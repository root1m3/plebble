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
#include "node_t.h"

#include <us/gov/config.h>
#include <iomanip>
#include <bitset>
#include <map>
#include "shards_t.h"
#include "db_t.h"

#define loglevel "test/sim"
#define logclass "node_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>


using namespace std;
using namespace us::sim;
using c = us::sim::node_t;

int c::edges_consensus{4};
int c::edges_network{8};

c::broadcast_ctl_t& c::broadcast_ctl_t::operator += (const broadcast_ctl_t& other) {
    n_redundant += other.n_redundant;
    n_effective += other.n_effective;
    n_relay_consensus += other.n_relay_consensus;
    n_relay_network += other.n_relay_network;
    n_ttl += other.n_ttl;
    n_unable_to_relay_network += other.n_unable_to_relay_network;
    return *this;
}

size_t c::broadcast_ctl_t::n_transmissions() const {
    return n_redundant + n_effective;
}

void c::broadcast_ctl_t::dump(ostream&os) const {
    os << "number of transmissions " << n_transmissions() << '\n';
    os << "    effective " << n_effective << '\n';
    os << "    redundant " << n_redundant << '\n';
    os << "    relay_consensus " << n_relay_consensus << '\n';
    os << "    relay_network " << n_relay_network << '\n';
    os << "    ttl " << n_ttl << '\n';
    os << "    unable_to_relay_network " << n_unable_to_relay_network << '\n';

}


c::node_t(): id(distu64(rng)) {
}

void c::dump_links(ostream& os) const {
    os << "Consensus " << links_consensus.size() << " Net " << links_network.size() << '\n';
}

void c::dump_links(const string& pfx, ostream& os) const {
    dump_links_consensus(pfx, os);
    dump_links_network(pfx, os);
}

void c::dump_links_consensus(const string& pfx, ostream& os) const {
    string npfx = pfx + "    ";
    os << pfx << links_consensus.size() << " consensus links:\n";
    for (auto& i: links_consensus) {
        os << npfx << "id " << bitset<32>(i->id) << '\n';
    }
}

void c::dump_links_network(const string& pfx, ostream& os) const {
    string npfx = pfx + "    ";
    os << pfx << links_network.size() << " network links:\n";
    for (auto& i: links_network) {
        os << npfx << "shard key " << i->shard->key << " node-id " << bitset<32>(i->id) << '\n';
    }
}

void c::dump_1liner(ostream& os) const {
    os << "edges consensus " << setfill('0') << setw(2) << links_consensus.size() << " net " << setfill('0') << setw(2) << links_network.size() << " id " << bitset<32>(id) << '\n';
}

void c::dump_txdata(ostream& os) const {
    os << txdata.in << ' ' << txdata.out << '\n';
}

c::broadcast_ctl_t c::process_tx(const rtx_t& tx) {
    return process_tx(nullptr, tx);
}

c::broadcast_ctl_t c::process_tx(node_t* from, const rtx_t& rtx) {
    broadcast_ctl_t ctl;
    ctl.bf.push_back(make_tuple(from, this, rtx));
//    process_tx(from, copy, rtx);
    while (!ctl.bf.empty()) {
        get<1>(ctl.bf.front())->process_tx(ctl);
        ctl.bf.pop_front();
    }
    return move(ctl);
}

void c::process_tx(broadcast_ctl_t& ctl) {
    node_t* from = get<0>(ctl.bf.front());
    rtx_t& rtx = get<2>(ctl.bf.front());
    if (from != nullptr) {
        from->txdata.out += rtx.sersize;
    }
    txdata.in += rtx.sersize;


//    cout << "received tx node id " << bitset<32>(id) << " shard " << shard->key << endl;
    //++tx_arrived_tot;

    if (rtx.hit_target_shard(shard->key)) {
//        cout << "hit node in target shard " << shard->key << "\n";
//        if (from && (shard == from->shard)) {
//            ++tx_arrived_from_same_shard;
//        }
//        else {
//            ++tx_arrived_from_other_shard;
//        }
/*        
        assert(
            (shard->key.mask & *reinterpret_cast<const uint32_t*>(rtx.tx->from.data())) == shard->key.coord
            ||
            (shard->key.mask & *reinterpret_cast<const uint32_t*>(rtx.tx->to.data())) == shard->key.coord
        );
*/
        if (txs.find(rtx.tx->id) != txs.end()) {
//            cout << "stop relay\n";
            ++ctl.n_redundant;
            return;
        }
        ++ctl.n_effective;
//        cout << "1st seen tx\n";
        txs.emplace(rtx.tx->id);
        for (auto& i: links_consensus) {
            if (i == from) continue;
//            cout << "relay to " << bitset<32>(i->id) << '\n';
            ctl.bf.push_back(make_tuple(this, i, rtx));
            ++ctl.n_relay_consensus;
        }
        if (rtx.sersize == TX_SER_SZ) {
            process(*rtx.tx);
        }
        if(rtx.route.empty()) return;
    }
    if (--rtx.ttl < 0) {
//        cout << "TTL exceeded. stooping.\n";
        ++ctl.n_ttl;
        return;
    }
    assert(!rtx.route.empty());
//rtx.dump_route(shard->key, cout);
//dump_links_network_dist("", rtx.route.begin()->first, cout);


    map<node_t*, set<uint32_t>> m;
    find_relay_nodes(rtx.route, m);
    if (m.empty()) {
        ++ctl.n_unable_to_relay_network;
    }
    for (auto& i: m) {
        assert(!i.second.empty());
        //rtx.visited.emplace(i.second);
        //cout << i->shard->key << endl;
//i->dump_links_network("    ", cout);
        ctl.bf.emplace_back(make_tuple(this, i.first, rtx_t(rtx, move(i.second))));
        ++ctl.n_relay_network;
    }

//assert(false);
}

void c::process(const tx_t& tx) {

    assert(shard->db != nullptr);
    ++db_version;
    //cout << "process tx" << endl;
}

c::broadcast_ctl_t c::consensus_cycle1() {
    //cout << "consensus node " << bitset<32>(id) << endl;
    tx_t* tx = new tx_t(1);
    rtx_t rtx(tx, DELTAS_SZ, shard->key.coord);
    auto ret = process_tx(rtx);
    delete tx;
    return move(ret);
}

c::broadcast_ctl_t c::consensus_cycle2() {
    //cout << "consensus node " << bitset<32>(id) << endl;
    tx_t* tx = new tx_t(1);
    rtx_t rtx(tx, VOTE_SZ, shard->key.coord);
    auto ret = process_tx(rtx);
    delete tx;
    return move(ret);
}

void c::find_relay_nodes(uint32_t content, multimap<int, node_t*>& m) {
    //cout << "content " << bitset<32>(content) << endl;
//    m = links_network;
// dump_links_network("", cout);
    auto i = links_network.begin();
    while (i != links_network.end()) {
        auto mask = (*i)->shard->key.mask;
        auto cont = (content & mask);
        //cout << "XX shard key " << (*i)->shard->key << " node-id " << bitset<32>((*i)->id) << '\n';
        //cout << "XX mask " << bitset<32>(mask) << " cont " << bitset<32>(cont) << '\n';
        int l = 0;
        while ((mask & 1) == 1) {
            //cout << "l = " << l << endl;
            if ((((*i)->id >> l) & 1) != (cont & 1)) {
                break;
            }
            mask >>= 1;
            cont >>= 1;
            ++l;
        }
        if (l > 0) m.emplace(l, *i);
        ++i;
    }

//    cout << "After filter: " << endl;
//    for (auto& i: m) {
//        cout << "l=" << i.first << " shard key " << i.second->shard->key << " node-id " << bitset<32>(i.second->id) << '\n';
//    }
/*
        for (auto& n: links_network) {
            //if (visited.find(n) != visited.end()) continue;

            auto dist = ((n->shard->key.mask & content) ^ n->shard->key.coord);
            if (dist < contentdist) {
                contentdist = dist;
                m.emplace(dist, n);
            }
        }
        
        
    }

    for (auto& n: links_network) {
        //if (visited.find(n) != visited.end()) continue;

        auto dist = ((n->shard->key.mask & content) ^ n->shard->key.coord);
        if (dist < contentdist) {
            contentdist = dist;
            m.emplace(dist, n);
        }
    }
*/
}

void c::find_relay_nodes(const set<uint32_t>& content, map<node_t*, set<uint32_t>>& m) {
    for (auto& i: content) {
        multimap<int, node_t*> n;
        find_relay_nodes(i, n);
        int count = 1;
        auto t = n.rbegin();
        while(t != n.rend() && count > 0) {
            //cout << t->first << ' ' << t->second->shard->key << endl;
            auto k = m.find(t->second);
            if (k == m.end()) {
                k = m.emplace(t->second, set<uint32_t>()).first;
            }
            k->second.emplace(i);
//            m.emplace(t->second);
            ++t;
            --count;
        }
    }
}


bool c::same_shard(const node_t& other) const {
    return (shard == other.shard);
}

bool c::connect0_consensus(node_t& other) {
    if (this == &other) {
//        cout << "!self-connect" << endl;
        return false; //self-connect
    }
    if (!same_shard(other)) {
//        cout << "! not same shard" << endl;
        return false; //self-connect
    }
    if (links_consensus.size() > (edges_consensus/2)) {
//        cout << "! my links_consensus are full" << endl;
        return false;
    }
    if (links_consensus.find(&other) != links_consensus.end()) {
//        cout << "! already connected" << endl;
        return false;
    }
    links_consensus.emplace(&other);
    other.links_consensus.emplace(this);
    return true;
}

bool c::connect0_network(node_t& other) {
    if (this == &other) {
//        cout << "!self-connect" << endl;
        return false; //self-connect
    }
    if (same_shard(other)) {
//        cout << "! same shard" << endl;
        return false; //self-connect
    }
//    if (links_network.size() > (edges_network/2)) {
//        cout << "! my links_network are full" << endl;
//        return false;
//    }
    if (links_network.find(&other) != links_network.end()) {
//        cout << "! already connected" << endl;
        return false;
    }
    links_network.emplace(&other);
    other.links_network.emplace(this);
    return true;
}


