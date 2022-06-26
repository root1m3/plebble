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
#include "net.h"
#include <us/gov/config.h>
#include <bitset>
#include "db_t.h"

#define loglevel "test/sim"
#define logclass "net_t"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::sim;
using c = us::sim::net_t;

c::net_t(size_t N) {
//    vector<node_t*> rnv; //temporary vector containing all nodes for building the connected mesh
//    rnv.reserve(N);
    for (size_t i = 0; i < N; ++i) {
        auto n = new node_t();
        if (i%1000000==0) cout << i << endl;
        emplace(n);
//        rnv.emplace_back(n);
    }
    cout << "Setting node db" << endl;
    apply([](node_t& n, shard_t& s) {
        n.shard = &s;
    });
    cout << "links" << endl;
    size_t p = 0;
    apply([&](node_t& n, shard_t& s) {
        uniform_int_distribution<mt19937::result_type> dist(0, s.db->nodes.size() - 1); // distribution in range [0, peer's negihbor list sz -1)
        int tries = 10;
        while (n.links_consensus.size() < (n.edges_consensus / 2)) {
            auto i = dist(rng);
            if (n.connect0_consensus(*s.db->nodes[i])) continue;
            if (--tries == 0) break;
        }

        {
            set<node_t*> m;
            routing_nodes(n.id, m);
            for (auto& i: m) {
                n.connect0_network(*i);
            }
        }

        if (++p%100000==0) cout << p << endl;
    });
    //dump0(cout);
//    cout << "links net" << endl;
    //random_shuffle(rnv.begin(), rnv.end());
    //auto rn = rnv.begin();
/*
    p = 0;
    apply([&](node_t& n, shard_t& s) {
        
        assert(false);
      / *  


        int tries = 10;
        while (n.links_network.size() < (n.edges_network / 2)) {
            auto t = *rn; //random_node(); //finds one random node on the network
            ++rn;
            if (rn == rnv.end()) rn = rnv.begin();
            if (t->shard == &s) continue;
            if (n.connect0_network(*t)) continue;
            if (--tries == 0) break;
        }
        if (++p%1000000==0) cout << p << endl;
* /
    });
*/
  //  rnv.clear();
}

void c::routing_nodes(uint64_t id, set<node_t*>& m) {
    auto s = root();
    assert(s != nullptr);
//    cout << "routing_nodes for node id " << bitset<32>(id) << endl;
    while(true) { //traverse tree from root to myself. Pick one node from every divergent branch
//        s->dump_1liner(cout);
//        string ind = string(4+s->depth()*4, ' ');
//        cout << ind << (s->db != nullptr ? "db" : "tree") << " shard key " << s->key << endl;
        if (s->db != nullptr) {
            if ((s->key.mask & id) != s->key.coord) {
                auto n = s->db->random_node();
//                cout << "node " << n->id << " shard key " << s->key << endl;
                m.emplace(n);
            }
            break;
//            cout << ind << "db here" << endl;
        }
        
        assert (s->shard0 != nullptr);
        auto x = (s->key.mask & id);
//cout << "x = " << bitset<32>(x) << endl;
        if (x == 0) {
            auto n = s->shard1->random_node();
            m.emplace(n);
//            cout << "node from branch where bit is 1: " <<  bitset<32>(n->id) << " shard key " << s->key << endl;
//            cout << "new shard path 0\n";
            s = s->shard0;
        }
        else {
            auto n = s->shard0->random_node();
            m.emplace(n);
//            cout << "node from branch where bit is 0: " <<  bitset<32>(n->id) << " shard key " << s->key << endl;
//            cout << "new shard path 1\n";
            s = s->shard1;
        }
    }
}

node_t::broadcast_ctl_t c::consensus_cycle() {
    node_t::broadcast_ctl_t ctl;
    cout << "consensus_cycle1" << endl;
    apply(
        [&](node_t& n) {
            auto m = n.consensus_cycle1();
            cout << "node consensus cycle 1" << endl;
            m.dump(cout);
            ctl += m;
        }
    );
    cout << "consensus_cycle2" << endl;
    apply(
        [&](node_t& n) {
            auto m = n.consensus_cycle2();
            cout << "node consensus cycle 2" << endl;
            m.dump(cout);
            ctl += m;
        }
    );
    apply(
        [&](db_t& db) {
        }
    );
/*
    {
        for (auto& n: *this) {
            tx_t tx(1, DELTAS_SZ); //pass off as deltas
            ctl += n.process_tx(tx);
        }
    } 
    {
        for (auto& n: *this) {
            tx_t tx(1, VOTE_SZ); //pass off as vote
            ctl += n.process_tx(tx);
        }
    } 
*/
    //stats_txs();
    return move(ctl);
}

