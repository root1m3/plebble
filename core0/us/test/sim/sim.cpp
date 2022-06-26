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
#include "sim.h"
#include <us/gov/config.h>
#include "net.h"

#define loglevel "test"
#define logclass "sim"
#include <us/test/logs.inc>

#include <us/test/assert.inc>

using namespace std;
using namespace us::sim;


/*
net_t::stats_t tx_broadcast(size_t n_nodes) {
    net_t net(n_nodes);
//    net.stats_links();
    cout << "TX Broadcast." << endl;
    auto n = net.random_node();
    tx_t tx(1, TX_SER_SZ);
    auto ctl = n->process_tx(tx);
    ctl.dump(cout);

    net_t::stats_t stats;
/ *
    stats.avg_transmissions_per_node = (((double)ctl.n_transmissions()) / ((double)n_nodes));
    net.stats_txs();
    net.stats_txdata(stats);
    net.stats_edges(stats);
* /
    return move(stats);
}

net_t::stats_t consensus(size_t n_nodes) {
    net_t net(n_nodes);
    node_t::broadcast_ctl_t ctl = net.consensus_cycle();
    ctl.dump(cout);

    net_t::stats_t stats;
/ *
    stats.avg_transmissions_per_node = (((double)ctl.n_transmissions()) / ((double)n_nodes));
    net.stats_txs();
    net.stats_txdata(stats);
    net.stats_edges(stats);
* /
    return move(stats);
}

void tx_broadcast(const string& pfx, ostream& os) {
    os << pfx << "TX broadcast." << endl;
    os << pfx << "=============" << endl;
    os << pfx << "#Nodes(network_size) avg_num_edges avg_transmissions_per_node avg_txdata_in_per_node avg_txdata_out_per_node\n";
    for (size_t n: {10, 100, 1000, 10000, 100000}) {
        auto st = tx_broadcast(n);
//        os << pfx << " nodes " << n << "; ";
//
assert(false);
//        st.dump_1liner(os);
    }
}

void consensus(const string& pfx, ostream& os) {
    os << pfx << "Consensus." << endl;
    os << pfx << "==========" << endl;
    os << pfx << "#Nodes(network_size) avg_num_edges avg_transmissions_per_node avg_txdata_in_per_node avg_txdata_out_per_node\n";
assert(false);
    for (size_t n: {10, 100, 1000, 10000, 100000}) {
        auto st = consensus(n);
//        os << pfx << " nodes " << n << "; ";
//        st.dump_1liner(os);
    }
}

void sim_main(ostream& os) {
    os << '\n';
    tx_broadcast("    ", os);
    consensus("    ", os);
}
*/

//mt19937 us::sim::rng(std::chrono::system_clock::now().time_since_epoch().count());
//uniform_int_distribution<mt19937::result_type> us::sim::distu64(0, numeric_limits<uint64_t>::max()); // distribution in range [0, N)

us::sim::clock us::sim::clock::instance;

void us::sim::sim_main() {
  
    net_t net(30);


    for (int i=0; i<100; ++i) {
        cout << "TX Broadcast." << endl;
        {
            auto n = net.random_node();
            tx_t* tx = new tx_t(1);
            rtx_t rtx(tx, TX_SER_SZ);
            auto ctl = n->process_tx(rtx);
            delete tx;
            ctl.dump(cout);
        }
    }
    {
        cout << "consensus..." << endl;
        auto ctl = net.consensus_cycle();
        ctl.dump(cout);
        
    }

    cout << "Computing network stats..." << endl;
    auto st = net.stats();
    st.dump(cout);


    //net.dump0(cout);

    //net.dump(cout);
//    net.stats_txs();

/*
    net_t::stats_t stats;
    stats.avg_transmissions_per_node = (((double)ctl.n_transmissions()) / ((double)n_nodes));
    net.stats_txs();
    net.stats_txdata(stats);
    net.stats_edges(stats);
    return move(stats);


    auto st = tx_broadcast(n, tries_link);
    os << pfx << " nodes " << n << "; ";
    st.dump_1liner(os);
*/
/*
    ostringstream os;
    sim_main(os);
    cout << "Results" << endl;
    cout << os.str() << endl;
*/
}


