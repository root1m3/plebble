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
#include "local_delta.h"
#include "app.h"

#define loglevel "gov/cash"
#define logclass "delta"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::delta;
using us::ko;

c::delta(): m(new engine::majority_merger<local_delta>) {
}

c::~delta() {
    delete m;
}

void c::dump(const string& prefix, ostream& os) const {
    g.dump(prefix, os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(g);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(g);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(g);
        if (is_ko(r)) return r;
    }
    return ok;
}

uint64_t c::merge(engine::app::local_delta* other0) {
    local_delta* other = static_cast<local_delta*>(other0);
    m->merge(*other);
    b::merge(other0);
    return 0;
}

void c::end_merge() {
    m->end_merge(g);
}

