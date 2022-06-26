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

#include <string>
#include <functional>
#include <iostream>
#include "maskcoord_t.h"

namespace us::sim {

    using namespace std;

    struct node_t;
    struct db_t;

    struct shards_t {

        struct stats_t {
            struct stdata {
                stdata(const string& name);
                stdata& operator += (size_t value);
                void dump(const string& pfx, ostream& os) const;
                size_t num;
                size_t acc;
                size_t min;
                size_t max;
                double avg;
                string name;
            };

            void dump(ostream& os) const;

            size_t nodes{0};
            size_t accounts{0};
            size_t shards{0};
            size_t edges_consensus{0};
            size_t edges_network{0};
            size_t transmissions{0};
            size_t txdata_in{0};
            size_t txdata_out{0};

            size_t tx_arrived_tot{0};
            size_t tx_arrived_from_same_shard{0};
            size_t tx_arrived_from_other_shard{0};
            size_t tx_relayed_network{0};
            size_t tx_relayed_consensus{0};
            size_t dummy{0};


            size_t nodes_per_shard{0};
            size_t accounts_per_shard{0};
            size_t transmissions_per_node{0};
            size_t txdata_in_per_node{0};
            size_t txdata_out_per_node{0};
            size_t edges_consensus_per_node{0};
            size_t edges_network_per_node{0};
        };

        shards_t();
        shards_t(shards_t* parent);
        virtual ~shards_t();

        stats_t stats() const;
        void stats(stats_t& st) const;

        node_t* random_node();
        void apply(const function<void(node_t&)>& f) const;
        void apply(const function<void(node_t&, shards_t&)>& f);
        void apply(const function<void(db_t&)>& f);

        bool emplace(node_t*);
        bool emplace0(node_t*);
        void dump0(ostream& os) const;
        void dump(ostream& os) const;
        void dump_1liner(ostream& os) const;
        void dump(const string& pfx, int& n, ostream&) const;
        int depth() const;
        shards_t* root();

    public:
        maskcoord_t key;

    public:
        db_t* db{nullptr};

    public:
        shards_t* shard0;
        shards_t* shard1;

    public:
        shards_t* parent;
    };

    using shard_t = shards_t;

}

std::ostream& operator << (std::ostream& os, const us::sim::shards_t::stats_t::stdata& o);

