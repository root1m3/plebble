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

#include <deque>
#include <map>
#include <set>
#include <tuple>
#include "tx_t.h"

namespace us::sim {

    using namespace std;

    struct shards_t;
    using shard_t = shards_t;
    using distance_t = uint32_t;
    struct db_t;

    struct node_t {

        struct broadcast_ctl_t {
            broadcast_ctl_t& operator += (const broadcast_ctl_t&);
            size_t n_transmissions() const;
            void dump(ostream&os) const;

        public:
            deque<tuple<node_t*, node_t*, rtx_t>> bf;
            size_t n_redundant{0};
            size_t n_effective{0};
            size_t n_relay_consensus{0};
            size_t n_relay_network{0};
            size_t n_ttl{0};
            size_t n_unable_to_relay_network{0};
        };

        struct txdata_t {
            size_t in{0};
            size_t out{0};
        };

    public:
        node_t();
        bool connect(node_t& n);
        bool connect0_consensus(node_t& n);
        bool connect0_network(node_t& n);
        bool same_shard(const node_t& other) const;
        void dump_links(ostream& os) const;
        void dump_links(const string& pfx, ostream& os) const;
        void dump_links_consensus(const string& pfx, ostream& os) const;
        void dump_links_network(const string& pfx, ostream& os) const;
        void dump_1liner(ostream& os) const;
        void dump_txdata(ostream& os) const;
        broadcast_ctl_t process_tx(const rtx_t& tx);
        broadcast_ctl_t process_tx(node_t* from, const rtx_t& rtx);
        void process_tx(broadcast_ctl_t& ctl);
//        multimap<distance_t, db_t*> find_relay_nodes(const set<uint32_t>& routes);
//        void find_relay_nodes(uint32_t route, multimap<distance_t, db_t*>&);
        void find_relay_nodes(uint32_t content, multimap<int, node_t*>&);
        void find_relay_nodes(const set<uint32_t>&,  map<node_t*, set<uint32_t>>&);
        void process(const tx_t& tx);
        broadcast_ctl_t consensus_cycle1();
        broadcast_ctl_t consensus_cycle2();



    public:
        uint64_t id;
        
        //map<uint8_t, set<node_t*>> links;
        set<node_t*> links_consensus;
        set<node_t*> links_network;
        set<uint64_t> txs;
        txdata_t txdata;
        static int edges_consensus;
        static int edges_network;

/*
        size_t tx_arrived_tot{0};
        size_t tx_arrived_from_same_shard{0};
        size_t tx_arrived_from_other_shard{0};
        size_t tx_relayed_network{0};
        size_t tx_relayed_consensus{0};
*/

        shard_t* shard{nullptr};
        int db_version{0};
    };

}

