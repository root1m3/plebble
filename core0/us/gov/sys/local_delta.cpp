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
#include "local_delta.h"
#include "app.h"

#define loglevel "gov/sys"
#define logclass "local_delta"
#include <us/gov/logs.inc>

using namespace us::gov;
using namespace us::gov::sys;
using c = us::gov::sys::local_delta;

size_t std::hash<us::gov::sys::local_delta>::operator() (const us::gov::sys::local_delta& g) const {
    return *reinterpret_cast<const size_t*>(&g.get_hash()[0]);
}

appid_t c::app_id() const {
    return app::id();
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    h.write(id);
    sensor.hash_data_to_sign(h);
}

void c::hash_data(hasher_t& h) const {
    h.write(id);
    sensor.hash_data(h);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "id " << id << '\n';
}

c::local_delta() {
}

c::~local_delta() {
}

const hash_t& c::get_hash() const {
    if (hash.is_zero()) hash = compute_hash();
    return hash;
}

hash_t c::compute_hash() const {
    hasher_t h;
    sensor.hash_data(h);
    hash_t v;
    h.finalize(v);
    return move(v);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(id) + blob_writer_t::blob_size(sensor);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(id);
    writer.write(sensor);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(id);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(sensor);
        if (is_ko(r)) return r;
    }
    return ok;
}

