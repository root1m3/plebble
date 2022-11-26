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
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>

#include "types.h"
#include "sigcode_section_t.h"

#define loglevel "cash"
#define logclass "sigcode_section"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::sigcode_section_t;

void c::dump(const string& prefix, ostream& os) const {
    {
        ostringstream pfix;
        pfix << prefix << " input ";
        inputs.dump(pfix.str(), os);
    }
    {
        ostringstream pfix;
        pfix << prefix << " output ";
        outputs.dump(pfix.str(), os);
    }
}

void c::dump_line(ostream& os) const {
    os << "i ";
    inputs.dump_line(os);
    os << "o ";
    outputs.dump_line(os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(static_cast<const blob_t&>(inputs)) + blob_writer_t::blob_size(static_cast<const blob_t&>(outputs));
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(static_cast<const blob_t&>(inputs));
    writer.write(static_cast<const blob_t&>(outputs));
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(static_cast<blob_t&>(inputs));
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(static_cast<blob_t&>(outputs));
        if (is_ko(r)) return r;
    }
    return ok;
}

void c::side_t::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << (int)i << '\n';
    }
}

void c::side_t::dump_line(ostream& os) const {
    for (auto& i: *this) {
        os << (int)i << ' ';
    }
}

