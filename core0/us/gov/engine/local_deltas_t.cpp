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
#include "local_deltas_t.h"

#include <us/gov/io/seriable.h>
#include <us/gov/engine/protocol.h>
#include <us/gov/engine/db_t.h>

#include "types.h"
#include "diff.h"

#define loglevel "gov/engine"
#define logclass "local_deltas"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::local_deltas_t;

c::~local_deltas_t() {
    clear();
}

void c::clear() {
    for (auto& i: *this) delete i.second;
    b::clear();
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    h.write(id);
    for (auto& i: *this) {
        h.write(i.first);
        i.second->hash_data_to_sign(h);
    }
}

void c::hash_data(hasher_t& h) const {
    h.write(id);
    for (auto& i: *this) {
        h.write(i.first);
        i.second->hash_data(h);
    }
}

void c::dumpX(ostream& os) const {
    os << "local deltas sz " << size() << '\n';
    os << "local deltas tsid " << id << '\n';
    for (auto& i: *this) {
        ostringstream pfx;
        pfx << "appid " << i.first << ": ";
        i.second->dump(pfx.str(), os);
    }
}

size_t c::blob_size() const {
    size_t sz = signed_data::blob_size() +
        blob_writer_t::blob_size(id) +
        blob_writer_t::sizet_size(size());
    for (auto& i: *this) {
        sz += blob_writer_t::blob_size(i.first);
        sz += blob_writer_t::blob_size(*i.second);
    }
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    assert(signature.is_not_zero());
    signed_data::to_blob(writer);
    writer.write(id);
    writer.write_sizet(size());
    for (auto& i: *this) {
        writer.write(i.first);
        writer.write(*i.second);
    }
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = signed_data::from_blob(reader);
        if (is_ko(r)) return r;
        if (unlikely(signature.is_zero())) {
            auto r = "KO 30299 Empty signature.";
            log(r);
            return r;
        }
    }
    {
        auto r = reader.read(id);
        if (is_ko(r)) return r;
    }
    clear();
    uint64_t sz;
    {
        auto r = reader.read_sizet(sz);
        if (is_ko(r)) return r;
        if (unlikely(sz > db_t::num_apps)) {
            auto r = "KO 91211 Too many apps.";
            log(r, sz);
            return r;
        }
    }
    for (uint64_t i = 0; i < sz; ++i) {
        appid_t appid;
        {
            auto r = reader.read(appid);
            if (is_ko(r)) return r;
        }
        app::local_delta* v = app::local_delta::create(appid);
        if (v == nullptr) {
            auto r = "KO 40392 Failed loading local_delta for app";
            log(r);
            return r;
        }
        {
            auto r = reader.read(*v);
            if (is_ko(r)) {
                delete v;
                return r;
            }
        }
        emplace(appid, v);
    }
    return ok;
}

datagram* c::get_datagram(channel_t channel, seq_t seq) const {
    return s::get_datagram(channel, protocol::engine_local_deltas, seq);
}

