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
#include "sigcode_t.h"
#include <us/gov/crypto/base58.h>
#include "types.h"

#define loglevel "gov/cash"
#define logclass "sigcode"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::sigcode_t;

size_t c::blob_size() const {
    return blob_writer_t::blob_size(ts) + blob_writer_t::blob_size(static_cast<const b&>(*this));
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(ts);
    writer.write(static_cast<const b&>(*this));
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(ts);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(static_cast<b&>(*this));
        if (is_ko(r)) return r;
    }
    return ok;
}

void c::dump(ostream& os) const {
    os << "exec time: " << (ts ? "bounded" : "changeable") << '\n'; //bounded = included in signature
    os << "bounded entries:\n";
    int s = 0;
    for (auto& i: *this) {
        ostringstream pfix;
        pfix << "section " << s;
        i.dump(pfix.str(), os);
        ++s;
    }
}

void c::dump_line(ostream& os) const {
    os << (ts ? "[ts]" : "[!ts]") << ' '; //bounded = included in signature
    int s = 0;
    for (auto& i: *this) {
        os << 's' << s << "{";
        i.dump_line(os);
        os << "} ";
        ++s;
    }
}

