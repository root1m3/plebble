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
#include <us/gov/io/cfg0.h>
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>

#include "readable.h"

#define loglevel "gov/io"
#define logclass "readable"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::readable;

ko c::read(const blob_t& blob) {
    blob_reader_t reader(blob);
    auto serid = serial_id();
    log("readable::read from blob", blob.size(), "serid", +serid);
    if (serid != 0) {
        auto r = reader.read_header(serid);
        if (is_ko(r)) {
            return r;
        }
    }
    return reader.read(*this);
}

ko c::read(const string& blob_b58) {
    log("readable::read from encoded blob");
    return read(us::gov::crypto::b58::decode(blob_b58));
}

ko c::read(const datagram& d) {
    log("readable::read from datagram");
    blob_reader_t reader(d);
    auto serid = serial_id();
    if (serid != 0) {
        auto r = reader.read_header(serid);
        if (is_ko(r)) {
            return r;
        }
    }
    return reader.read(*this);
}

ko c::load(const string& filename) {
    log("readable::load", filename);
    blob_t blob;
    {
        auto r = io::read_file_(filename, blob);
        if (is_ko(r)) {
            return r;
        }
    }
    return read(blob);
}

pair<ko, c::blob_header_t> c::read1(const blob_t& blob) {
    pair<ko, blob_header_t> ret;
    blob_reader_t reader(blob);
    auto serid = serial_id();
    if (serid != 0) {
        auto r = reader.read_header(serid);
        if (is_ko(r)) {
            ret.first = r;
            return move(ret);
        }
    }
    ret.first = reader.read(*this);
    ret.second = reader.header;
    return move(ret);
}

pair<ko, c::blob_header_t> c::read1(const string& blob_b58) {
    return read1(us::gov::crypto::b58::decode(blob_b58));
}

pair<ko, c::blob_header_t> c::load1(const string& filename) {
    pair<ko, blob_header_t> ret;
    blob_t blob;
    {
        auto r = io::read_file_(filename, blob);
        if (is_ko(r)) {
            ret.first = r;
            return move(ret);
        }
    }
    return read1(blob);
}

pair<ko, blob_t> c::load3(const string& filename) {
    pair<ko, blob_t> ret;
    {
        auto r = io::read_file_(filename, ret.second);
        if (is_ko(r)) {
            ret.first = r;
            return move(ret);
        }
    }
    ret.first = ok;
    return move(ret);
}

pair<ko, pair<c::blob_header_t, hash_t>> c::read2(const string& blob_b58) {
    return read2(us::gov::crypto::b58::decode(blob_b58));
}

pair<ko, pair<c::blob_header_t, hash_t>> c::read2(const blob_t& blob) {
    pair<ko, pair<blob_header_t, hash_t>> ret;
    {
        auto r = read1(blob);
        if (is_ko(r.first)) {
            ret.first = r.first;
            return move(ret);
        }
        ret.second.first = move(r.second);
    }
    ret.second.second = hasher_t::digest(blob);
    return move(ret);
}

pair<ko, pair<c::blob_header_t, hash_t>> c::load2(const string& filename) {
    blob_t blob;
    {
        auto r = io::read_file_(filename, blob);
        if (is_ko(r)) {
            pair<ko, pair<blob_header_t, hash_t>> ret;
            ret.first = r;
            return move(ret);
        }
    }
    return read2(blob);
}

pair<ko, readable*> c::load(const string& filename, function<readable*(const serid_t&)> f) {
    pair<ko, readable*> ret;
    blob_t blob;
    {
        auto r = io::read_file_(filename, blob);
        if (is_ko(r)) {
            ret.first = r;
            ret.second = nullptr;
            return move(ret);
        }
    }
    blob_reader_t reader(blob);
    auto r = reader.read_header();
    if (is_ko(r)) {
        ret.first = r;
        return move(ret);
    }
    ret.second = f(reader.header.serid);
    if (ret.second == nullptr) {
        ret.first = "KO 40091 factory not found for blob type.";
        log(ret.first);
        return move(ret);
    }
    ret.first = reader.read(*ret.second);
    if (is_ko(ret.first)) {
        delete ret.second;
        ret.second = nullptr;
    }
    return move(ret);
}

