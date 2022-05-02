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
#include "db_t.h"

#define loglevel "gov/sys"
#define logclass "db_t"
#include <us/gov/logs.inc>

using c = us::gov::sys::db_t;
using namespace us::gov;
using namespace std;

c::db_t() {
    clear();
}

c::db_t(c&& other) {
    sensors = other.sensors;
}

c::~db_t() {
}

void c::dump(const string& prefix, ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << prefix << sensors.size() << " sensors:\n";
    sensors.dump(prefix + "    ", os);
}

void c::clear() {
    lock_guard<mutex> lock(mx);
    sensors.clear();
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(sensors);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(sensors);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(sensors);
        if (is_ko(r)) return r;
    }
    return ok;
}

