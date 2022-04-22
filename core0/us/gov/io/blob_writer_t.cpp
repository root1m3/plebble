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
#include "blob_writer_t.h"
#include <cstring>
#include <us/gov/crypto/base58.h>
#include <us/gov/socket/types.h>

#define loglevel "gov/io"
#define logclass "blob_writer_t"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::blob_writer_t;

c::blob_writer_t(blob_t& blob, const size_t& sz): blob(blob) {
    blob.resize(sz);
    cur = blob.data();
}

c::blob_writer_t(datagram& d): blob(d) {
    cur = blob.data() + datagram::h;
}

size_t c::sizet_size(const uint64_t& sz) {
    if (sz < 0xfd) { //8bit
        return sizeof(uint8_t);
    }
    if (sz <= 0xffff) { //16bit
        return sizeof(uint8_t) + sizeof(uint16_t);
    }
    if (sz <= 0xffffffff) { //32bit
        return sizeof(uint8_t) + sizeof(uint32_t);
    }
    return sizeof(uint8_t) + sizeof(uint64_t); //64bit
}

void c::write_sizet(const uint64_t& sz) {
    if (sz < 0xfd) { //8bit
        log("write at blob offset", (uint64_t)(cur - blob.data()));
        *cur = (uint8_t)sz;
        ++cur;
        return;
    }
    if (sz <= 0xffff) { //16bit
        log("write at blob offset", (uint64_t)(cur - blob.data()));
        *cur = 0xfd;
        ++cur;
        uint16_t x = (uint16_t)sz;
        memcpy(cur, &x, sizeof(uint16_t));
        log("write at blob offset", (uint64_t)(cur - blob.data()));
        cur += sizeof(uint16_t);
        return;
    }
    if (sz <= 0xffffffff) { //32bit
        log("write at blob offset", (uint64_t)(cur - blob.data()));
        *cur = 0xfe;
        ++cur;
        uint32_t x = (uint32_t)sz;
        memcpy(cur, &x, sizeof(uint32_t));
        log("write at blob offset", (uint64_t)(cur - blob.data()));
        cur += sizeof(uint32_t);
        return;
    }
    //64bit
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    *cur = 0xff;
    ++cur;
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &sz, sizeof(uint64_t));
    cur += sizeof(uint64_t);
}

template<> void c::write(const bool& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    *cur = o ? (uint8_t)1 : (uint8_t)0;
    cur += sizeof(uint8_t);
}

template<> void c::write(const int64_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &o, sizeof(int64_t));
    cur += sizeof(int64_t);
}

template<> void c::write(const int32_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &o, sizeof(int32_t));
    cur += sizeof(int32_t);
}

template<> void c::write(const uint8_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    *cur = o;
    cur += sizeof(uint8_t);
}

template<> void c::write(const uint16_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &o, sizeof(int16_t));
    cur += sizeof(uint16_t);
}

template<> void c::write(const uint32_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &o, sizeof(uint32_t));
    cur += sizeof(uint32_t);
}

template<> void c::write(const uint64_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, &o, sizeof(uint64_t));
    cur += sizeof(uint64_t);
}

template<> void c::write(const time_point& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    uint64_t x = duration_cast<chrono::nanoseconds>(o.time_since_epoch()).count();
    memcpy(cur, &x, sizeof(uint64_t));
    cur += sizeof(uint64_t);
}


template<> size_t c::blob_size(const string& o) {
    return sizet_size(o.size()) + o.size();
}

template<> void c::write(const string& o) {
    write_sizet(o.size());
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, o.data(), o.size());
    cur += o.size();
}


template<> size_t c::blob_size(const string_view& o) {
    return sizet_size(o.size()) + o.size();
}

template<> void c::write(const string_view& o) {
    write_sizet(o.size());
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, o.data(), o.size());
    cur += o.size();
}


template<> size_t c::blob_size(const blob_t& o) {
    return sizet_size(o.size()) + o.size();
}

template<> void c::write(const blob_t& o) {
    write_sizet(o.size());
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, o.data(), o.size());
    cur += o.size();
}


template<> void c::write(const char& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    *cur = o;
    ++cur;
}

template<> void c::write(const hash_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, o.data(), hasher_t::output_size);
    cur += hasher_t::output_size;
}

template<> void c::write(const sigmsg_hash_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    memcpy(cur, o.data(), sigmsg_hasher_t::output_size);
    cur += sigmsg_hasher_t::output_size;
}

template<> void c::write(const pub_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    o.write(cur);
    cur += o.ser_size;
}

template<> void c::write(const priv_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    o.write(cur);
    cur += o.ser_size;
}

template<> size_t c::blob_size(const sig_t& o) {
    return sig_t::mem_size;
}

template<> void c::write(const sig_t& o) {
    log("write at blob offset", (uint64_t)(cur - blob.data()));
    o.write(cur);
    cur += o.mem_size;
}

void c::write_header(const serid_t& serid) {
    log("write_header", +serid, '=', serid);
    assert(serid != 0);
    log("writing blob header", +blob_reader_t::current_version, +serid);
    write(blob_reader_t::current_version);
    write(serid);
}

datagram* c::get_datagram(channel_t channel, svc_t svc, seq_t seq) {
    return new datagram(channel, svc, seq, 0);
}


datagram* c::get_datagram(channel_t channel, svc_t svc, seq_t seq, const blob_t& blob) {
    log("get_datagram from blob", svc, seq, blob.size());
    auto d = new datagram(channel, svc, seq, blob.size());
    memcpy(d->data() + datagram::h, blob.data(), blob.size());
    return d;
}


blob_t c::make_blob(const string& payload) {
    log("blob from string");
    blob_t blob;
    blob_writer_t w(blob, blob_size(payload));
    w.write(payload);
    return move(blob);
}

//--writable------------------------------

size_t c::writable::tlv_size() const {
    return (serial_id() != 0 ? header_size() : 0) + blob_size();
}

void c::writable::write(blob_t& blob) const {
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

void c::writable::write(string& b58) const {
    log("writable::write to encoded string");
    blob_t blob;
    write(blob);
    b58 = crypto::b58::encode(blob);
}

string c::writable::encode() const {
    string s;
    write(s);
    return move(s);
}

ko c::writable::save(const string& filename) const {
    log("writable::save", filename);
    blob_t blob;
    write(blob);
    return io::write_file_(blob, filename);
}

datagram* c::writable::get_datagram(channel_t channel, svc_t svc, seq_t seq) const {
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

