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
#include <mutex>

#include <us/gov/engine/app.h>
#include <us/gov/peer/account_t.h>
#include <us/gov/peer/nodes_t.h>
#include <us/gov/engine/peer_t.h>
#include <us/gov/io/seriable.h>

namespace us::gov::engine::auth {

    using account_t = peer::account_t;
    using nodes_t = peer::nodes_t;
    struct maskcoord_t;

    struct db_t final: io::seriable {

        db_t(nodes_t& nodes, mutex& mxnodes, nodes_t& hall, mutex& mxhall);

        peer_t::stage_t get_stage(const hash_t&) const;
        void hash(hasher_t&) const;
        void dump(const string& prefix, ostream&) const;
        void clear();
        void dump_as_seeds(ostream&) const;
        void collect(vector<tuple<hash_t, uint32_t, uint16_t>>&) const;
        bool is_node(const hash_t& pkh) const;
        pair<ko, account_t> lookup(const hash_t&) const;
        void filter(const maskcoord_t&);
        void filter(nodes_t&, const maskcoord_t&);

    public:
        size_t blob_size() const final override;
        void to_blob(blob_writer_t&) const final override;
        ko from_blob(blob_reader_t&) final override;

    public:
        mutex& mx_nodes;
        nodes_t& nodes;

        mutex& mx_hall;
        nodes_t& hall;
    };

}

