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
#include <cstring>

#include <us/gov/crypto/base58.h>
#include <us/gov/socket/types.h>
#include <us/gov/io/cfg0.h>

#include "blob_writer_t.h"
#include "writable.h"

#define loglevel "gov/io"
#define logclass "writable"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::writable;

size_t c::tlv_size() const {
    return (serial_id() != 0 ? header_size() : 0) + blob_size();
}

void c::write(blob_t& blob) const {
    log("writable::write to blob");
    auto serid = serial_id();
    auto sz = (serid != 0 ? header_size() : 0) + blob_size();
    if (sz == 0) {
        blob.clear();
        return;
    }
    blob_writer_t w(blob, sz);
    if (serid != 0) {
        w.write_header(serid);
    }
    to_blob(w);
    assert(w.cur == blob.data() + sz);
}

void c::write(string& b58) const {
    log("writable::write to encoded string");
    blob_t blob;
    write(blob);
    b58 = crypto::b58::encode(blob);
}

string c::encode() const {
    string s;
    write(s);
    return move(s);
}

ko c::save(const string& filename) const {
    log("writable::save", filename);
    blob_t blob;
    write(blob);
    return io::write_file_(blob, filename);
}

datagram* c::get_datagram(channel_t channel, svc_t svc, seq_t seq) const {
    log("writable::get_datagram svc", svc);
    auto serid = serial_id();
    auto sz = (serid != 0 ? header_size() : 0) + blob_size();
    auto d = new datagram(channel, svc, seq, sz);
    blob_writer_t w(*d);
    if (sz == 0) {
        return d;
    }
    if (serid != 0) {
        w.write_header(serid);
    }
    w.write(*this);
    assert(w.cur == d->data() + d->size());
    return d;
}

