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
#include "track_status_t.h"

#define loglevel "gov/engine"
#define logclass "track_status_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::track_status_t;

void c::reset(const ts_t& ifrom, const ts_t& ito, const evt_status_t& ist, const string& iinfo) {
    from = ifrom;
    to = ito;
    st = ist;
assert(iinfo.empty());
    info = iinfo;
}

void c::reset(const ts_t& ifrom, const ts_t& ito, const evt_status_t& ist) {
    from = ifrom;
    to = ito;
    st = ist;
}

void c::reset(const ts_t& ts, const evt_status_t& ist) {
    from = ts;
    to = ts + 1;
    st = ist;
}

void c::reset(const ts_t& ts, const evt_status_t& ist, const string& iinfo) {
    from = ts;
    to = ts + 1;
    st = ist;
assert(iinfo.empty());
    info = iinfo;
}

void c::dump(ostream& os) const {
    if (range()) {
        os << from << '-' << to << ' ';
    }
    else {
        os << from << ' ';
    }
    os << (int)st << ' ' << evt_status_str[st] << ' ' << info << '\n';
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(from) +
        blob_writer_t::blob_size(to) +
        blob_writer_t::blob_size((uint8_t) st) +
        blob_writer_t::blob_size(info);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(from);
    writer.write(to);
    writer.write((uint8_t) st);
    writer.write(info);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(from);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(to);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read((uint8_t&)st);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(info);
        if (is_ko(r)) return r;
    }
    return ok;
}

