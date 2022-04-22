//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
#include "delta.h"

#include <thread>
#include <chrono>
#include <fstream>

#include <us/gov/vcs.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/engine/net_daemon_t.h>
#include <us/gov/likely.h>

#include "install_script.h"
#include "install_script_response.h"
#include "types.h"

#define loglevel "gov/sys"
#define logclass "delta"
#include <us/gov/logs.inc>

using namespace us::gov::sys;
using c = us::gov::sys::delta;

c::delta() {
}

c::~delta() {
}

void c::dump(const string& prefix, ostream& os) const {
    g.dump(prefix, os);
}

uint64_t c::merge(engine::app::local_delta* other0) {
    local_delta* other = static_cast<local_delta*>(other0);
    m.emplace(other->id, other->sensor);
    log("merge local_delta src=", other->id, "new size", m.size());
    b::merge(other0);
    return 0;
}

void c::end_merge() {
    log("end_merge. total sensors received", m.size());
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(m);
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(m);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(m);
        if (is_ko(r)) return r;
    }
    return ok;
}

