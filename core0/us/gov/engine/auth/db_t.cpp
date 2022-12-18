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
#include <us/gov/config.h>
#include <us/gov/engine/peer_t.h>

#include "maskcoord_t.h"
#include "types.h"
#include "db_t.h"

#define loglevel "gov/engine/auth"
#define logclass "db_t"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine::auth;
using c = us::gov::engine::auth::db_t;

c::db_t(nodes_t& n, mutex& mn, nodes_t& h, mutex& mh): nodes(n), mx_nodes(mn), hall(h), mx_hall(mh) {
}

bool c::is_node(const hash_t& pkh) const {
    lock_guard<mutex> lock(mx_nodes);
    return nodes.find(pkh) != nodes.end();
}

us::gov::engine::peer_t::stage_t c::get_stage(const hash_t& key) const {
    {
        lock_guard<mutex> lock(mx_nodes);
        if (nodes.find(key) != nodes.end()) return peer_t::node;
    }
    {
        lock_guard<mutex> lock(mx_hall);
        if (hall.find(key) != hall.end()) return peer_t::hall;
    }
    return peer_t::out;
}

#ifdef CFG_TOPOLOGY_MESH
    size_t c::blob_size() const { //nodes and hall must be locked by the caller
        return blob_writer_t::blob_size(nodes) + blob_writer_t::blob_size(hall);
    }

    void c::to_blob(blob_writer_t& writer) const { //nodes and hall must be locked by the caller
        writer.write(nodes);
        writer.write(hall);
    }

    ko c::from_blob(blob_reader_t& reader) {
        {
            lock_guard<mutex> lock(mx_nodes);
            auto r = reader.read(nodes);
            if (is_ko(r)) return r;
        }
        {
            lock_guard<mutex> lock(mx_hall);
            auto r = reader.read(hall);
            if (is_ko(r)) return r;
        }
        return ok;
    }

    void c::clear() {
        {
            lock_guard<mutex> lock(mx_nodes);
            nodes.clear();
        }
        {
            lock_guard<mutex> lock(mx_hall);
            hall.clear();
        }
    }

#endif

#ifdef CFG_TOPOLOGY_RING
    size_t c::blob_size() const {
        return 0;
    }

    void c::to_blob(blob_writer_t&) const {
    }

    ko c::from_blob(blob_reader_t&) {
        return ok;
    }

    void c::clear() {}
#endif

void c::dump(const string& prefix, ostream& os) const {
    string pfx = prefix + "    ";
    {
        lock_guard<mutex> lock(mx_nodes);
        os << prefix << nodes.size() << " nodes:\n";
        nodes.dump(pfx, os);
    }
    {
        lock_guard<mutex> lock(mx_hall);
        os << prefix << hall.size() << " candidates in hall:\n";
        hall.dump(pfx, os);
    }
}

void c::dump_as_seeds(ostream& os) const {
    lock_guard<mutex> lock(mx_nodes);
    nodes.dump_as_seeds(os);
}

void c::collect(vector<tuple<hash_t, uint32_t, uint16_t>>& v) const {
    lock_guard<mutex> lock(mx_nodes);
    nodes.collect(v);
}

void c::hash(hasher_t& h) const {
    {
        lock_guard<mutex> lock(mx_nodes);
        for (auto& i: nodes) {
            h << i.first;
            i.second.hash(h);
        }
    }
    {
        lock_guard<mutex> lock(mx_hall);
        for (auto& i: hall) {
            h << i.first;
            i.second.hash(h);
        }
    }
}

pair<ko, account_t> c::lookup(const hash_t& h) const {
    {
        lock_guard<mutex> lock(mx_nodes);
        auto i = nodes.lookup(h);
        if (i.first == ok) return i;
    }
    lock_guard<mutex> lock(mx_hall);
    return hall.lookup(h);
}

void c::filter(nodes_t& n, const maskcoord_t& maskcoord) {
    auto i = n.begin();
    while (i != n.end()) {
        if ((i->first.lsdw() & maskcoord.mask) == maskcoord.coord) {
            i = n.erase(i);
        }
        else {
            ++i;
        }
    }
}

void c::filter(const maskcoord_t& maskcoord) {
    filter(nodes, maskcoord);
    filter(hall, maskcoord);
}


