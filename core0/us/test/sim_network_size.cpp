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
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <map>
#include <deque>
#include <sstream>

#define TX_SER_SZ 600 //avg size in bytes of a serialized transaction
#define DELTAS_SZ 10000 //avg size in bytes of a delta packet (part of consensus)
#define VOTE_SZ 160 //avg size in bytes of a vote packet (part of consensus)

using namespace std;

struct net_t;

struct account {
    int balance;
};

struct db: map<size_t, account> {

};

struct tx_t {
    tx_t(const size_t& from, const size_t& to, int amount, size_t sersize): id(next_id++), from(from), to(to), amount(amount), sersize(sersize) {
    }
    size_t from;
    size_t to;
    int amount;
    size_t sersize; //size in bytes when serialized
    size_t id;

    static size_t next_id;
};

size_t tx_t::next_id{0};

struct node_t {

    struct broadcast_ctl_t {
        deque<pair<node_t*, node_t*>> bf;
        size_t n_redundant{0};
        size_t n_effective{0};

        broadcast_ctl_t& operator += (const broadcast_ctl_t& other) {
            n_redundant += other.n_redundant;
            n_effective += other.n_effective;
            return *this;
        }

        size_t n_transmissions() const { return n_redundant + n_effective; }

        void dump(ostream&os) const {
            cout << "number of transmissions " << n_transmissions() << '\n';
            cout << "    effective " << n_effective << '\n';
            cout << "    redundant " << n_redundant << '\n';
        }
    };

    struct txdata_t {
        size_t in{0};
        size_t out{0};
    };

    node_t() {
    }

    size_t memsize() const {
        return sizeof(id) + links.capacity() * sizeof(node_t*);
    }

    void connect(node_t& n);

    void dump_links(ostream& os) const {
        os << links.size() << '\n';
    }

    void dump_txs(ostream& os) const {
        os << txs.size() << '\n';
    }

    void dump_txdata(ostream& os) const {
        os << txdata.in << ' ' << txdata.out << '\n';
    }

    void clean_seen() {
        txs.clear();
        txdata.in = txdata.out = 0;
    }

    broadcast_ctl_t process_tx(tx_t& tx) {
        return process_tx(nullptr, tx);
    }

    broadcast_ctl_t process_tx(node_t* from, tx_t& tx) {
        broadcast_ctl_t ctl;
        process_tx(from, tx, ctl);
        while (!ctl.bf.empty()) {
            ctl.bf.front().second->process_tx(ctl.bf.front().first, tx, ctl);
            ctl.bf.pop_front();
        }
        return move(ctl);
    }

    void process_tx(node_t* from, const tx_t& tx, broadcast_ctl_t& ctl) {
        if (from != nullptr) {
            from->txdata.out += tx.sersize;
        }
        txdata.in += tx.sersize;
        auto i = txs.find(tx.id);
        if (i != txs.end()) {
            ++ctl.n_redundant;
            return;
        }
        txs.emplace(tx.id);
        ++ctl.n_effective;
        for (auto& i: links) {
            if (from != i) { //don't sent the tx back to the source node
                ctl.bf.push_back(make_pair(this, i));
            }
        }
    }

    using hash_t = uint64_t;

    hash_t id;
    vector<node_t*> links;
    set<int> txs;
    txdata_t txdata;
};


struct net_t: vector<node_t> {

    struct stats_t {
        double avg_transmissions_per_node{0};
        double avg_txdata_in_per_node{0};
        double avg_txdata_out_per_node{0};
        double avg_edges_per_node{0};

        void dump(ostream& os) const {
            os << "stats: \n";
            os << "avg_edges_per_node " << avg_edges_per_node << '\n';
            os << "avg_transmissions_per_node " << avg_transmissions_per_node << '\n';
            os << "avg_txdata_in_per_node " << avg_txdata_in_per_node << '\n';
            os << "avg_txdata_out_per_node " << avg_txdata_out_per_node << '\n';
        }

        void dump_1liner(ostream& os) const {
            os << "edges " << avg_edges_per_node << "; sent_datagrams " << avg_transmissions_per_node << "; IN-bytes " << avg_txdata_in_per_node << "; out-bytes " << avg_txdata_out_per_node << '\n';
        }

    };

    net_t(size_t N, int tries_link) {
        resize(N);
        ifstream f("/dev/urandom");
        if (!f.good()) {
            auto r = "KO 43021 Opening entropy file /dev/urandom";
            cerr << r << endl;
            exit(1);
        }
        for (auto& i: *this) {
            f.read(reinterpret_cast<char*>(&i.id), sizeof(i.id));
        }

        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> dist(0, size() - 1); // distribution in range [0, N)

        for (auto& i: *this) {
            for (int l = 0; l < tries_link; ++l) {
                i.connect((*this)[dist(rng)]);
            }
        }
    }

    node_t& random_node() {
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> dist(0, size() - 1); // distribution in range [0, N)
        return (*this)[dist(rng)];
    }

    size_t memsize() const {
        size_t s{0};
        for (auto& i: *this) {
            s += i.memsize();
        }
        return s;
    }

    void dump_links(ostream& os) const {
        for (auto& i: *this) {
            i.dump_links(os);
        }
    }

    void dump_txs(ostream& os) const {
        for (auto& i: *this) {
            i.dump_txs(os);
        }
    }

    void stats_links() const {
        {
            ofstream os("/tmp/data");
            dump_links(os);
        }
        cout << "Nodes by number of links" << endl;
        cout << "#links #nodes" << endl;
        system("cat /tmp/data | sort | uniq -c | awk '{ print $2\" \"$1 }' | sort -n > /tmp/data1; cat /tmp/data1");
        
        cout << "average number of links" << endl;
        system("cat /tmp/data1 | awk '{ total += $1*$2; count+=$2 } END { print total/count }'");
    }

    void stats_txs() const {
        {
            ofstream os("/tmp/data");
            dump_txs(os);
        }
        cout << "Nodes by Txs received" << endl;
        cout << "#tx #nodes" << endl;
        system("cat /tmp/data | sort | uniq -c | awk '{ print $2\" \"$1 }' | sort -n");
    }

    void stats_edges(stats_t& st) const {
        st.avg_edges_per_node = 0;
        for (auto& n: *this) {
            st.avg_edges_per_node += n.links.size();
        }
        st.avg_edges_per_node /= (double) size();
    }

    void stats_txdata(stats_t& st) const {
        st.avg_txdata_in_per_node = 0;
        st.avg_txdata_out_per_node = 0;
        for (auto& n: *this) {
            st.avg_txdata_in_per_node += n.txdata.in;
            st.avg_txdata_out_per_node += n.txdata.out;
        }
        st.avg_txdata_in_per_node /= (double) size();
        st.avg_txdata_out_per_node /= (double) size();
    }

    void clean_seen() {
        for (auto& i: *this) {
            i.clean_seen();
        }
    }

    node_t::broadcast_ctl_t consensus_cycle() {
        node_t::broadcast_ctl_t ctl;
        {
            for (auto& n: *this) {
                tx_t tx(23, 67, 1, DELTAS_SZ); //pass off as deltas
                ctl += n.process_tx(tx);
            }
        } 
        {
            for (auto& n: *this) {
                tx_t tx(11, 22, 1, VOTE_SZ); //pass off as vote
                ctl += n.process_tx(tx);
            }
        } 
        //stats_txs();
        return move(ctl);
    }

};


void node_t::connect(node_t& other) {
    links.push_back(&other);    
    other.links.push_back(this);    
}

net_t::stats_t tx_broadcast(size_t n_nodes, size_t tries_link) {
    net_t net(n_nodes, tries_link);
    cout << "network memory bytes " << net.memsize() << endl;
    net.stats_links();
    cout << "TX Broadcast." << endl;
    auto& n = net.random_node();
    tx_t tx(45, 77, 1, TX_SER_SZ);
    auto ctl = n.process_tx(tx);
    ctl.dump(cout);

    net_t::stats_t stats;
    stats.avg_transmissions_per_node = (((double)ctl.n_transmissions()) / ((double)n_nodes));
    net.stats_txs();
    net.stats_txdata(stats);
    net.stats_edges(stats);
    return move(stats);
}

net_t::stats_t consensus(size_t n_nodes, size_t tries_link) {
    net_t net(n_nodes, tries_link);
    net_t::stats_t stats;
    node_t::broadcast_ctl_t ctl = net.consensus_cycle();
    ctl.dump(cout);
    stats.avg_transmissions_per_node = (((double)ctl.n_transmissions()) / ((double)n_nodes));
    net.stats_txs();
    net.stats_txdata(stats);
    net.stats_edges(stats);
    return move(stats);
}

void tx_broadcast(size_t tries_link, const string& pfx, ostream& os) {
    os << pfx << "TX broadcast." << endl;
    os << pfx << "=============" << endl;
    os << pfx << "#Nodes(network_size) avg_num_edges avg_transmissions_per_node avg_txdata_in_per_node avg_txdata_out_per_node\n";
    for (size_t n: {10, 100, 1000, 10000, 100000, 1000000}) {
        auto st = tx_broadcast(n, tries_link);
        os << pfx << " nodes " << n << "; ";
        st.dump_1liner(os);
    }
}

void consensus(size_t tries_link, const string& pfx, ostream& os) {
    os << pfx << "Consensus." << endl;
    os << pfx << "==========" << endl;
    os << pfx << "#Nodes(network_size) avg_num_edges avg_transmissions_per_node avg_txdata_in_per_node avg_txdata_out_per_node\n";
    for (size_t n: {10, 100, 1000, 10000, 100000, 1000000}) {
        auto st = consensus(n, tries_link);
        os << pfx << " nodes " << n << "; ";
        st.dump_1liner(os); 
    }
}


void sim_main(size_t tries_link, ostream& os) {
    os << '\n';
    os << "tries_link/node=" << tries_link << " ( =" << (2*tries_link) << " edges)" << endl;
    tx_broadcast(tries_link, "    ", os);
    consensus(tries_link, "    ", os);
}

int main() {
    ostringstream os;
    //sim_main(1, os);
    //sim_main(2, os);
    sim_main(3, os);
    sim_main(6, os);
    sim_main(10, os);
    cout << "Results" << endl;
    cout << os.str() << endl;
    return 0;
}

