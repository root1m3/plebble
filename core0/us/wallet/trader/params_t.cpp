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
#include "params_t.h"
#include "ch_t.h"

#define loglevel "wallet/trader"
#define logclass "params_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::local_params_t;

void c::load(const string& dir, ch_t& ch) { //returns true on full or partial load
    log("load params from dir", dir);
    ch.priv_params_changed |= priv.loadX(dir + "/params_private");
    ch.shared_params_changed |= shared.loadX(dir + "/params_shared");
}

void c::save(const string& dir) {
    us::gov::io::cfg0::ensure_dir(dir);
    priv.saveX(dir + "/params_private");
    shared.saveX(dir + "/params_shared");
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "private:\n";
    priv.dump(prefix + "  ", os);
    os << prefix << "shared:\n";
    shared.dump(prefix + "  ", os);
}

void c::clear() {
    priv.clear();
    shared.clear();
}

size_t c::blob_size() const {
    auto sz = priv.blob_size() + shared.blob_size();
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    writer.write(priv);
    writer.write(shared);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        auto r = reader.read(priv);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(shared);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

