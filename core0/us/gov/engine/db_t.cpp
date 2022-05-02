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

#include <us/gov/config.h>
#include <us/gov/sys/app.h>
#include <us/gov/cash/app.h>
#include <us/gov/traders/app.h>

#include "daemon_t.h"
#include "auth/app.h"

#define loglevel "gov/engine"
#define logclass "db_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::db_t;

c::db_t(engine::daemon_t& d) {
    auth_app = new engine::auth::app(d);
    add(auth_app);
    sys_app = new sys::app(d);
    add(sys_app);
    cash_app = new cash::app(d);
    add(cash_app);
    traders_app = new traders::app(d);
    add(traders_app);
    assert(apps.size() == num_apps);
}

void c::add(app* app) {
    #ifdef DEBUG
    if (apps.find(app->get_id()) != apps.end()) {
        cerr << "Fatal: App collision.\n";
        exit(1);
    }
    #endif
    apps.emplace(app->get_id(), app);
}

void c::dump(int detail, ostream& os) const {
    os << "last_delta_imported_id " << last_delta_imported_id << '\n';
    apps.full_dump("", detail, os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(last_delta_imported_id) +
    blob_writer_t::blob_size(apps);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(last_delta_imported_id);
    writer.write(apps);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(last_delta_imported_id);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(apps);
        if (is_ko(r)) return r;
    }
    return ok;
}

