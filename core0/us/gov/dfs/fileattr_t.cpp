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
#include "fileattr_t.h"
#include <us/gov/io/cfg0.h>

#define loglevel "gov/dfs"
#define logclass "fileattr_t"
#include "logs.inc"

using namespace us::gov::dfs;
using c = us::gov::dfs::fileattr_t;
using us::ko;

c::fileattr_t(): sz(0) {
}

c::fileattr_t(const string& path, uint32_t sz): path(us::gov::io::cfg0::rewrite_path(path)), sz(sz) {
}

void c::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    h.write(path);
    h.write(sz);
}

void c::hash_data(crypto::ripemd160& h) const {
    h.write(path);
    h.write(sz);
}

void c::dump(ostream& os) const {
    os << path << ' ' << sz << " bytes\n";
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(sz) + blob_writer_t::blob_size(path);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(sz);
    writer.write(path);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(sz);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(path);
        if (is_ko(r)) return r;
    }
    return ok;
}

