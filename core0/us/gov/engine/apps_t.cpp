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
#include "apps_t.h"
#include "app.h"

#define loglevel "gov/engine"
#define logclass "apps_t"
#include "logs.inc"

using c = us::gov::engine::apps_t;
using namespace us::gov::engine;
using namespace std;
using us::ko;

c::~apps_t() {
    for (auto& i: *this) {
        delete i.second;
    }
}

ko c::shell_command(appid_t app_id, istream& is, ostream& os) {
    auto i = find(app_id);
    if (i == end()) return "KO 60594 app not found.";
    return i->second->shell_command(is, os);
}

void c::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << +i.first << '\t' << i.second->get_name() << '\n';
    }
}

void c::full_dump(const string& prefix, int detail, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << "app " << +i.first << ' ' << i.second->get_name() << ":\n";
        i.second->dump(prefix + "    ", detail, os);
    }
}

size_t c::blob_size() const {
    size_t sz = 0;
    for (auto& i: *this) {
        sz += blob_writer_t::blob_size(*i.second);
    }
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    for (auto& i: *this) {
        writer.write(*i.second);
    }
}

ko c::from_blob(blob_reader_t& reader) {
    for (auto& i: *this) {
        auto r = reader.read(*i.second);
        if (is_ko(r)) return r;
    }
    return ok;
}

