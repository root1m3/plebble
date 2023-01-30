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

#include "blob_reader_t.h"

#define loglevel "gov/io"
#define logclass "blob_reader_t"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::blob_reader_t;

const char* c::KO_67217 = "KO 67217 Overflow.";
const char* c::KO_60498 = "KO 60498 Invalid blob object.";
const char* c::KO_75643 = "KO 75643 sizet exceeded max limit.";

c::blob_reader_t(const blob_t& blob): blob(blob), cur(blob.data()), end(blob.data() + blob.size()) {
}

c::blob_reader_t(const datagram& d): blob(d), cur(blob.data() + datagram::h), end(blob.data() + blob.size()) {
    assert(d.size() >= datagram::h);
}

ko c::read_sizet(uint64_t& o) {
    uint8_t pfx;
    {
        auto r = read(pfx);
        if (is_ko(r)) return r;
    }
    if (pfx < 0xfd) {
        o = pfx;
        return ok;
    }
    if (pfx == 0xfd) {
        uint16_t v;
        auto r = read(v);
        if (is_ko(r)) return r;
        o = v;
        return ok;
    }
    if (pfx == 0xfe) {
        uint32_t v;
        auto r = read(v);
        if (is_ko(r)) return r;
        o = v;
        return ok;
    }
    return read(o);
}

template<> ko c::read(bool& o) {
    if ((cur + sizeof(uint8_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    uint8_t x;
    x = *cur;
    cur += sizeof(uint8_t);
    o = (x != 0);
    return ok;
}

template<> ko c::read(uint8_t& o) {
    if ((cur + sizeof(uint8_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o = *cur;
    cur += sizeof(uint8_t);
    return ok;
}

template<> ko c::read(uint16_t& o) {
    if ((cur + sizeof(uint16_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(&o, cur, sizeof(uint16_t));
    cur += sizeof(uint16_t);
    return ok;
}

template<> ko c::read(uint32_t& o) {
    if ((cur + sizeof(uint32_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(&o, cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);
    return ok;
}

template<> ko c::read(uint64_t& o) {
    if ((cur + sizeof(uint64_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(&o, cur, sizeof(uint64_t));
    cur += sizeof(uint64_t);
    return ok;
}

template<> ko c::read(int64_t& o) {
    if ((cur + sizeof(int64_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(&o, cur, sizeof(int64_t));
    cur += sizeof(int64_t);
    return ok;
}

template<> ko c::read(int32_t& o) {
    if ((cur + sizeof(int32_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(&o, cur, sizeof(int32_t));
    cur += sizeof(int32_t);
    return ok;
}

template<> ko c::read(char& o) {
    if ((cur + sizeof(char)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o = *cur;
    ++cur;
    return ok;
}

template<> ko c::read(time_point& o) {
    if ((cur + sizeof(uint64_t)) > end) {
        log(KO_67217);
        return KO_67217;
    }
    uint64_t x; //Use memcpy and not reinterpret_cast. ARM processors will fail when parsing packed structures (blob) if variables are not aligned in memory
    memcpy(&x, cur, sizeof(uint64_t));
    o = time_point(chrono::duration_cast<chrono::system_clock::duration>(chrono::nanoseconds(x)));
    cur += sizeof(uint64_t);
    return ok;
}

template<> ko c::read(string& o) {
    uint64_t sz;
    auto e = read_sizet(sz);
    if (is_ko(e)) return e;
    if ((cur + sz) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o = string(cur, cur + sz);
    cur += sz;
    return ok;
}

template<> ko c::read(hash_t& o) {
    if ((cur + hasher_t::output_size) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(o.data(), cur, hasher_t::output_size);
    cur += hasher_t::output_size;
    return ok;
}

template<> ko c::read(sigmsg_hash_t& o) {
    if ((cur + sigmsg_hasher_t::output_size) > end) {
        log(KO_67217);
        return KO_67217;
    }
    memcpy(o.data(), cur, sigmsg_hasher_t::output_size);
    cur += sigmsg_hasher_t::output_size;
    return ok;
}

template<> ko c::read(pub_t& o) {
    if ((cur + o.ser_size) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o.read(cur);
    cur += o.ser_size;
    return ok;
}

template<> ko c::read(priv_t& o) {
    if ((cur + o.ser_size) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o.read(cur);
    cur += o.ser_size;
    return ok;
}

template<> ko c::read(keys& o) {
    auto r = read(o.priv);
    if (is_ko(r)) return r;
    return read(o.pub);
}

template<> ko c::read(sig_t& o) {
    if (header.version == 7) {
        if ((cur + 1) > end) {
            log(KO_67217);
            return KO_67217;
        }
        auto b = *cur;
        ++cur;
        if (b == 0) { //v7 legit
            o.zero();
        }
        else if (b == 1) { //v7 legit
            if ((cur + o.mem_size) > end) {
                log(KO_67217);
                return KO_67217;
            }
            o.read(cur);
            cur += o.mem_size;
        }
        else { //v8 passing as v7
            --cur;
            if ((cur + o.mem_size) > end) {
                log(KO_67217);
                return KO_67217;
            }
            o.read(cur);
            cur += o.mem_size;
        }
        return ok;
    }
    if ((cur + o.mem_size) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o.read(cur);
    cur += o.mem_size;
    return ok;
}

template<> ko c::read(blob_t& o) {
    uint64_t sz;
    auto e = read_sizet(sz);
    if (is_ko(e)) return e;
    if ((cur + sz) > end) {
        log(KO_67217);
        return KO_67217;
    }
    o.resize(sz);
    memcpy(o.data(), cur, sz);
    cur += sz;
    return ok;
}

uint8_t c::peek() const {
    if (cur >= end) return 0;
    return *cur;
}

ko c::read_header(serid_t serid) {
    log("read_header", serid, +serid);
    {
        auto r = read(header.version);
        if (is_ko(r)) return r;
    }
    {
        auto r = read(header.serid);
        if (is_ko(r)) return r;
    }
    if (header.serid != serid) {
        log(KO_60498);
        return KO_60498;
    }
/*
    if (unlikely(header.version != current_version)) {
        log("Blob is not current version.", +header.version, +current_version);
        if (header.version == current_version - 1) {
            log("Blob is prev version.", +header.version, +current_version);
            return ok;
        }
        log(KO_60499, +header.version, +current_version);
        return KO_60499;
    }
*/
    return ok;
}

ko c::read_header() {
    log("read_header");
    {
        auto r = read(header.version);
        if (is_ko(r)) return r;
    }
    {
        auto r = read(header.serid);
        if (is_ko(r)) return r;
    }
/*
    if (unlikely(header.version != current_version)) {
        log("Blob is not current version.", +header.version, +current_version);
        if (header.version == current_version - 1) {
            log("Blob is prev version.", +header.version, +current_version);
            return ok;
        }
        log(KO_60499, +header.version, +current_version);
        return KO_60499;
    }
*/
    return ok;
}

ko c::read_header(const string& file, blob_header_t& header) {
    ifstream is(file, ios::binary);
    if (!is.good()) {
        return KO_60498;
    }
    is.get(reinterpret_cast<char&>(header.version));
    is.get(reinterpret_cast<char&>(header.serid));
    if (unlikely(!is.good())) {
        return KO_60498;
    }
/*
    if (unlikely(header.version != current_version)) {
        log("Blob is not current version.", +header.version, +current_version);
        if (header.version == current_version - 1) {
            log("Blob is prev version.", +header.version, +current_version);
            return ok;
        }
        log(KO_60499, +header.version, +current_version);
        return KO_60499;
    }
*/
    return ok;
}
/*
template<>
ko c::readD(const datagram& d, blob_t& o) {
    log("readD blob");
    o.resize(d.size() - datagram::h);
    memcpy(o.data(), d.data() + datagram::h, d.size() - datagram::h);
    return ok;
}

template<>
ko c::readD(const datagram& d, blob_reader_t::readable& o) {
    log("readD readable");
    return o.read(d);
}
*/
