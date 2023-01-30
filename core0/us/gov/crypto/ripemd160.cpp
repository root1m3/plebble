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
#include "ripemd160.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <us/gov/likely.h>
#include <us/gov/likely.h>
#include <us/gov/io/cfg0.h>

#include "base58.h"
#include "endian_rw.h"
#include "types.h"

#define loglevel "gov/crypto"
#define logclass "ripemd160"
#include "logs.inc"

using namespace us::gov::crypto;
using c = us::gov::crypto::ripemd160;

namespace {
    uint32_t inline f1(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
    uint32_t inline f2(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
    uint32_t inline f3(uint32_t x, uint32_t y, uint32_t z) { return (x | ~y) ^ z; }
    uint32_t inline f4(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
    uint32_t inline f5(uint32_t x, uint32_t y, uint32_t z) { return x ^ (y | ~z); }

    void inline Initialize(uint32_t* s) {
        s[0] = 0x67452301ul;
        s[1] = 0xEFCDAB89ul;
        s[2] = 0x98BADCFEul;
        s[3] = 0x10325476ul;
        s[4] = 0xC3D2E1F0ul;
    }

    uint32_t inline rol(uint32_t x, int i) { return (x << i) | (x >> (32 - i)); }

    void inline Round(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t f, uint32_t x, uint32_t k, int r) {
        a = rol(a + f + x + k, r) + e;
        c = rol(c, 10);
    }

    void inline R11(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f1(b, c, d), x, 0, r); }
    void inline R21(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f2(b, c, d), x, 0x5A827999ul, r); }
    void inline R31(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f3(b, c, d), x, 0x6ED9EBA1ul, r); }
    void inline R41(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f4(b, c, d), x, 0x8F1BBCDCul, r); }
    void inline R51(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f5(b, c, d), x, 0xA953FD4Eul, r); }

    void inline R12(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f5(b, c, d), x, 0x50A28BE6ul, r); }
    void inline R22(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f4(b, c, d), x, 0x5C4DD124ul, r); }
    void inline R32(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f3(b, c, d), x, 0x6D703EF3ul, r); }
    void inline R42(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f2(b, c, d), x, 0x7A6D76E9ul, r); }
    void inline R52(uint32_t& a, uint32_t b, uint32_t& c, uint32_t d, uint32_t e, uint32_t x, int r) { Round(a, b, c, d, e, f1(b, c, d), x, 0, r); }

    void Transform(uint32_t* s, const unsigned char* chunk) { // Perform a RIPEMD-160 transformation, processing a 64-byte chunk.
        uint32_t a1 = s[0], b1 = s[1], c1 = s[2], d1 = s[3], e1 = s[4];
        uint32_t a2 = a1, b2 = b1, c2 = c1, d2 = d1, e2 = e1;
        uint32_t w0 = ReadLE32(chunk + 0), w1 = ReadLE32(chunk + 4), w2 = ReadLE32(chunk + 8), w3 = ReadLE32(chunk + 12);
        uint32_t w4 = ReadLE32(chunk + 16), w5 = ReadLE32(chunk + 20), w6 = ReadLE32(chunk + 24), w7 = ReadLE32(chunk + 28);
        uint32_t w8 = ReadLE32(chunk + 32), w9 = ReadLE32(chunk + 36), w10 = ReadLE32(chunk + 40), w11 = ReadLE32(chunk + 44);
        uint32_t w12 = ReadLE32(chunk + 48), w13 = ReadLE32(chunk + 52), w14 = ReadLE32(chunk + 56), w15 = ReadLE32(chunk + 60);

        R11(a1, b1, c1, d1, e1, w0, 11);
        R12(a2, b2, c2, d2, e2, w5, 8);
        R11(e1, a1, b1, c1, d1, w1, 14);
        R12(e2, a2, b2, c2, d2, w14, 9);
        R11(d1, e1, a1, b1, c1, w2, 15);
        R12(d2, e2, a2, b2, c2, w7, 9);
        R11(c1, d1, e1, a1, b1, w3, 12);
        R12(c2, d2, e2, a2, b2, w0, 11);
        R11(b1, c1, d1, e1, a1, w4, 5);
        R12(b2, c2, d2, e2, a2, w9, 13);
        R11(a1, b1, c1, d1, e1, w5, 8);
        R12(a2, b2, c2, d2, e2, w2, 15);
        R11(e1, a1, b1, c1, d1, w6, 7);
        R12(e2, a2, b2, c2, d2, w11, 15);
        R11(d1, e1, a1, b1, c1, w7, 9);
        R12(d2, e2, a2, b2, c2, w4, 5);
        R11(c1, d1, e1, a1, b1, w8, 11);
        R12(c2, d2, e2, a2, b2, w13, 7);
        R11(b1, c1, d1, e1, a1, w9, 13);
        R12(b2, c2, d2, e2, a2, w6, 7);
        R11(a1, b1, c1, d1, e1, w10, 14);
        R12(a2, b2, c2, d2, e2, w15, 8);
        R11(e1, a1, b1, c1, d1, w11, 15);
        R12(e2, a2, b2, c2, d2, w8, 11);
        R11(d1, e1, a1, b1, c1, w12, 6);
        R12(d2, e2, a2, b2, c2, w1, 14);
        R11(c1, d1, e1, a1, b1, w13, 7);
        R12(c2, d2, e2, a2, b2, w10, 14);
        R11(b1, c1, d1, e1, a1, w14, 9);
        R12(b2, c2, d2, e2, a2, w3, 12);
        R11(a1, b1, c1, d1, e1, w15, 8);
        R12(a2, b2, c2, d2, e2, w12, 6);

        R21(e1, a1, b1, c1, d1, w7, 7);
        R22(e2, a2, b2, c2, d2, w6, 9);
        R21(d1, e1, a1, b1, c1, w4, 6);
        R22(d2, e2, a2, b2, c2, w11, 13);
        R21(c1, d1, e1, a1, b1, w13, 8);
        R22(c2, d2, e2, a2, b2, w3, 15);
        R21(b1, c1, d1, e1, a1, w1, 13);
        R22(b2, c2, d2, e2, a2, w7, 7);
        R21(a1, b1, c1, d1, e1, w10, 11);
        R22(a2, b2, c2, d2, e2, w0, 12);
        R21(e1, a1, b1, c1, d1, w6, 9);
        R22(e2, a2, b2, c2, d2, w13, 8);
        R21(d1, e1, a1, b1, c1, w15, 7);
        R22(d2, e2, a2, b2, c2, w5, 9);
        R21(c1, d1, e1, a1, b1, w3, 15);
        R22(c2, d2, e2, a2, b2, w10, 11);
        R21(b1, c1, d1, e1, a1, w12, 7);
        R22(b2, c2, d2, e2, a2, w14, 7);
        R21(a1, b1, c1, d1, e1, w0, 12);
        R22(a2, b2, c2, d2, e2, w15, 7);
        R21(e1, a1, b1, c1, d1, w9, 15);
        R22(e2, a2, b2, c2, d2, w8, 12);
        R21(d1, e1, a1, b1, c1, w5, 9);
        R22(d2, e2, a2, b2, c2, w12, 7);
        R21(c1, d1, e1, a1, b1, w2, 11);
        R22(c2, d2, e2, a2, b2, w4, 6);
        R21(b1, c1, d1, e1, a1, w14, 7);
        R22(b2, c2, d2, e2, a2, w9, 15);
        R21(a1, b1, c1, d1, e1, w11, 13);
        R22(a2, b2, c2, d2, e2, w1, 13);
        R21(e1, a1, b1, c1, d1, w8, 12);
        R22(e2, a2, b2, c2, d2, w2, 11);

        R31(d1, e1, a1, b1, c1, w3, 11);
        R32(d2, e2, a2, b2, c2, w15, 9);
        R31(c1, d1, e1, a1, b1, w10, 13);
        R32(c2, d2, e2, a2, b2, w5, 7);
        R31(b1, c1, d1, e1, a1, w14, 6);
        R32(b2, c2, d2, e2, a2, w1, 15);
        R31(a1, b1, c1, d1, e1, w4, 7);
        R32(a2, b2, c2, d2, e2, w3, 11);
        R31(e1, a1, b1, c1, d1, w9, 14);
        R32(e2, a2, b2, c2, d2, w7, 8);
        R31(d1, e1, a1, b1, c1, w15, 9);
        R32(d2, e2, a2, b2, c2, w14, 6);
        R31(c1, d1, e1, a1, b1, w8, 13);
        R32(c2, d2, e2, a2, b2, w6, 6);
        R31(b1, c1, d1, e1, a1, w1, 15);
        R32(b2, c2, d2, e2, a2, w9, 14);
        R31(a1, b1, c1, d1, e1, w2, 14);
        R32(a2, b2, c2, d2, e2, w11, 12);
        R31(e1, a1, b1, c1, d1, w7, 8);
        R32(e2, a2, b2, c2, d2, w8, 13);
        R31(d1, e1, a1, b1, c1, w0, 13);
        R32(d2, e2, a2, b2, c2, w12, 5);
        R31(c1, d1, e1, a1, b1, w6, 6);
        R32(c2, d2, e2, a2, b2, w2, 14);
        R31(b1, c1, d1, e1, a1, w13, 5);
        R32(b2, c2, d2, e2, a2, w10, 13);
        R31(a1, b1, c1, d1, e1, w11, 12);
        R32(a2, b2, c2, d2, e2, w0, 13);
        R31(e1, a1, b1, c1, d1, w5, 7);
        R32(e2, a2, b2, c2, d2, w4, 7);
        R31(d1, e1, a1, b1, c1, w12, 5);
        R32(d2, e2, a2, b2, c2, w13, 5);

        R41(c1, d1, e1, a1, b1, w1, 11);
        R42(c2, d2, e2, a2, b2, w8, 15);
        R41(b1, c1, d1, e1, a1, w9, 12);
        R42(b2, c2, d2, e2, a2, w6, 5);
        R41(a1, b1, c1, d1, e1, w11, 14);
        R42(a2, b2, c2, d2, e2, w4, 8);
        R41(e1, a1, b1, c1, d1, w10, 15);
        R42(e2, a2, b2, c2, d2, w1, 11);
        R41(d1, e1, a1, b1, c1, w0, 14);
        R42(d2, e2, a2, b2, c2, w3, 14);
        R41(c1, d1, e1, a1, b1, w8, 15);
        R42(c2, d2, e2, a2, b2, w11, 14);
        R41(b1, c1, d1, e1, a1, w12, 9);
        R42(b2, c2, d2, e2, a2, w15, 6);
        R41(a1, b1, c1, d1, e1, w4, 8);
        R42(a2, b2, c2, d2, e2, w0, 14);
        R41(e1, a1, b1, c1, d1, w13, 9);
        R42(e2, a2, b2, c2, d2, w5, 6);
        R41(d1, e1, a1, b1, c1, w3, 14);
        R42(d2, e2, a2, b2, c2, w12, 9);
        R41(c1, d1, e1, a1, b1, w7, 5);
        R42(c2, d2, e2, a2, b2, w2, 12);
        R41(b1, c1, d1, e1, a1, w15, 6);
        R42(b2, c2, d2, e2, a2, w13, 9);
        R41(a1, b1, c1, d1, e1, w14, 8);
        R42(a2, b2, c2, d2, e2, w9, 12);
        R41(e1, a1, b1, c1, d1, w5, 6);
        R42(e2, a2, b2, c2, d2, w7, 5);
        R41(d1, e1, a1, b1, c1, w6, 5);
        R42(d2, e2, a2, b2, c2, w10, 15);
        R41(c1, d1, e1, a1, b1, w2, 12);
        R42(c2, d2, e2, a2, b2, w14, 8);

        R51(b1, c1, d1, e1, a1, w4, 9);
        R52(b2, c2, d2, e2, a2, w12, 8);
        R51(a1, b1, c1, d1, e1, w0, 15);
        R52(a2, b2, c2, d2, e2, w15, 5);
        R51(e1, a1, b1, c1, d1, w5, 5);
        R52(e2, a2, b2, c2, d2, w10, 12);
        R51(d1, e1, a1, b1, c1, w9, 11);
        R52(d2, e2, a2, b2, c2, w4, 9);
        R51(c1, d1, e1, a1, b1, w7, 6);
        R52(c2, d2, e2, a2, b2, w1, 12);
        R51(b1, c1, d1, e1, a1, w12, 8);
        R52(b2, c2, d2, e2, a2, w5, 5);
        R51(a1, b1, c1, d1, e1, w2, 13);
        R52(a2, b2, c2, d2, e2, w8, 14);
        R51(e1, a1, b1, c1, d1, w10, 12);
        R52(e2, a2, b2, c2, d2, w7, 6);
        R51(d1, e1, a1, b1, c1, w14, 5);
        R52(d2, e2, a2, b2, c2, w6, 8);
        R51(c1, d1, e1, a1, b1, w1, 12);
        R52(c2, d2, e2, a2, b2, w2, 13);
        R51(b1, c1, d1, e1, a1, w3, 13);
        R52(b2, c2, d2, e2, a2, w13, 6);
        R51(a1, b1, c1, d1, e1, w8, 14);
        R52(a2, b2, c2, d2, e2, w14, 5);
        R51(e1, a1, b1, c1, d1, w11, 11);
        R52(e2, a2, b2, c2, d2, w0, 15);
        R51(d1, e1, a1, b1, c1, w6, 8);
        R52(d2, e2, a2, b2, c2, w3, 13);
        R51(c1, d1, e1, a1, b1, w15, 5);
        R52(c2, d2, e2, a2, b2, w9, 11);
        R51(b1, c1, d1, e1, a1, w13, 6);
        R52(b2, c2, d2, e2, a2, w11, 11);

        uint32_t t = s[0];
        s[0] = s[1] + c1 + d2;
        s[1] = s[2] + d1 + e2;
        s[2] = s[3] + e1 + a2;
        s[3] = s[4] + a1 + b2;
        s[4] = t + b1 + c2;
    }
}

c::ripemd160() : bytes(0) {
    Initialize(s);
}

void c::write(const value_type& data) {
    write(&data[0], output_size);
}

void c::write(const vector<uint8_t>& data) {
    if(unlikely(data.empty())) return;
    write(reinterpret_cast<const unsigned char*>(&data[0]), data.size());
}

void c::write(const unsigned char* data, size_t len) {
    const unsigned char* end = data + len;
    size_t bufsize = bytes % 64;
    if (bufsize && bufsize + len >= 64) { // Fill the buffer, and process it.
        memcpy(buf + bufsize, data, 64 - bufsize);
        bytes += 64 - bufsize;
        data += 64 - bufsize;
        Transform(s, buf);
        bufsize = 0;
    }
    while (end >= data + 64) { // Process full chunks directly from the source.
        Transform(s, data);
        bytes += 64;
        data += 64;
    }
    if (end > data) { // Fill the buffer with what remains.
        memcpy(buf + bufsize, data, end - data);
        bytes += end - data;
    }
}

void c::write(const string& data) {
    if(unlikely(data.empty())) return;
    write(reinterpret_cast<const uint8_t*>(&data[0]), data.size());
}

void c::write(bool data) {
    write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

void c::write(const double& v) {
    write(reinterpret_cast<const uint8_t*>(&v), sizeof(v));
}

void c::write(const uint64_t& data) {
    write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

void c::write(const int64_t& v) {
    write(reinterpret_cast<const uint8_t*>(&v), sizeof(v));
}

void c::write(const uint8_t& v) {
    write(&v, 1);
}

void c::write(const uint32_t& data) {
    write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
}

void c::write(const int32_t& v) {
    write(reinterpret_cast<const uint8_t*>(&v), sizeof(v));
}

const unsigned char c::pad[64] = {0x80};

void c::finalize(value_type& hash) {
    unsigned char sizedesc[8];
    WriteLE64(sizedesc, bytes << 3);
    write(pad, 1 + ((119 - (bytes % 64)) % 64));
    write(sizedesc, 8);
    WriteLE32(&hash[0], s[0]);
    WriteLE32(&hash[4], s[1]);
    WriteLE32(&hash[8], s[2]);
    WriteLE32(&hash[12], s[3]);
    WriteLE32(&hash[16], s[4]);
}

void c::finalize(unsigned char hash[output_size]) {
    unsigned char sizedesc[8];
    WriteLE64(sizedesc, bytes << 3);
    write(pad, 1 + ((119 - (bytes % 64)) % 64));
    write(sizedesc, 8);
    WriteLE32(hash, s[0]);
    WriteLE32(hash + 4, s[1]);
    WriteLE32(hash + 8, s[2]);
    WriteLE32(hash + 12, s[3]);
    WriteLE32(hash + 16, s[4]);
}

void c::reset() {
    bytes = 0;
    Initialize(s);
}

string c::value_type::to_hex() const {
    ostringstream os;
    for (int i = 0; i < output_size; ++i) {
        unsigned char x = (*this)[i];
        x >>= 4;
        int g;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
        x = (*this)[i] & 0x0F;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
    }
    return os.str();
}

c::value_type c::value_type::from_hex(const string& hex) {
    value_type k;
    if (hex.size() != 2 * output_size) {
        k.zero();
        return k;
    }
    int j = 0;
    istringstream is(hex);
    for (int i = 0; i < 2 * output_size; i += 2) {
        char x=::toupper(hex[i]);
        unsigned char v;
        if (x >= '0' && x <= '9') v = x - '0'; else v = 10 + (x - 'A');
        k[j] = v << 4;
        x= ::toupper(hex[i + 1]);
        if (x >= '0' && x <= '9') v = x - '0'; else v = 10 + (x - 'A');
        k[j] |= v;
        ++j;
    }
    return move(k);
}

bool c::value_type::set_hex(const string& hex) {
    if (hex.size() != 2 * output_size) return false;
    istringstream is(hex);
    int j = 0;
    for (int i = 0; i < 2 * output_size; i += 2) {
        char x= ::toupper(hex[i]);
        unsigned char v;
        if (x >= '0' && x <= '9') v = x - '0'; else v = 10 + (x - 'A');
        (*this)[j] = v << 4;
        x = ::toupper(hex[i+1]);
        if (x >= '0' && x <= '9') v = x - '0'; else v=10 + (x - 'A');
        (*this)[j]|=v;
        ++j;
    }
    return true;
}

string c::value_type::to_b58() const {
    return b58::encode(&(*this)[0], &(*this)[0] + output_size);
}

string c::value_type::encode() const {
    return b58::encode(&(*this)[0], &(*this)[0] + output_size);
}

string c::value_type::encode_path() const {
    auto s = encode();
    auto l = s.size() - 2;
    ostringstream os;
    os << s[0];
    for (size_t i = 1; i < l; ++i) {
        if (i % 2 == 0) {
            os << '/';
        }
        os << s[i];
    }
    return os.str();
}

pair<string, string> c::value_type::filename() const {
    auto s = encode();
    auto l = s.size() - 2;
    ostringstream os;
    os << s[0];
    for (size_t i = 1; i < l; ++i) {
        if (i % 2 == 0) {
            os << '/';
        }
        os << s[i];
    }
    return make_pair(os.str(), s);
}

c::value_type c::value_type::from_b58(const string& s) {
    value_type k;
    vector<unsigned char> v;
    if (unlikely(!b58::decode(s, v))) {
            k.zero();
            return k;
    }
    if (unlikely(v.size() != output_size)) { k.zero(); return k; }
    memcpy(&k[0], &v[0], output_size);
    return move(k);
}

bool c::value_type::set_b58(const string& s) {
    vector<unsigned char> v;
    if (unlikely(!b58::decode(s,v))) {
        zero();
        return false;
    }
    if (unlikely(v.size() != output_size)) { zero(); return false; }
    memcpy(&(*this)[0], &v[0], output_size);
    return true;
}

c::value_type::value_type() {
    zero(); //TODO it should be possible to remove this line wo side effects
}

c::value_type::value_type(unsigned int i) {
    set(i);
}

c::value_type::value_type(const string& b58) {
    set_b58(b58);
}

c::value_type::value_type(const value_type& other): b(other) {
}

size_t c::value_type::ser_sz() const {
    return c::output_size;
}

unsigned char* c::value_type::write_to(unsigned char*p) const {
    memcpy(p, &(*this)[0], c::output_size);
    return p + c::output_size;
}

ko c::value_type::read(istream& is) {
    is.read((char*)data(), c::output_size);
    if (!is.good()) {
        zero();
        return "KO 40329 Error reading hash from stream.";
    }
    return ok;
}

const unsigned char* c::value_type::read_from(const unsigned char* p, const unsigned char* bend) {
    if (p+c::output_size>bend) return nullptr;
    memcpy(&(*this)[0], p, c::output_size);
    return p + c::output_size;
}

void c::value_type::fill_random(mt19937& rng) {
    size_t p = 0;
    while (true) {
        *reinterpret_cast<mt19937::result_type*>(&(*this)[p]) = rng();
        p += sizeof(mt19937::result_type);
        if (p == output_size) break;
        if (p > output_size) {
            p = output_size - sizeof(mt19937::result_type);
            *reinterpret_cast<mt19937::result_type*>(&(*this)[p]) = rng();
            break;
        }
    }
}

ko c::value_type::fill_random() {
    ifstream f("/dev/urandom");
    if (!f.good()) {
        auto r = "KO 43021 Opening entropy file /dev/urandom";
        log(r);
        zero();
        return r;
    }
    f.read(reinterpret_cast<char*>(data()), c::output_size);
    return ok;
}

uint32_t c::value_type::uint32() const {
    return *reinterpret_cast<const uint32_t*>(&(*this)[16]);
}

void c::value_type::set(unsigned int i) {
    *reinterpret_cast<uint64_t*>(&(*this)[0]) = *reinterpret_cast<uint64_t*>(&(*this)[8]) = 0;
    *reinterpret_cast<uint32_t*>(&(*this)[16]) = i;
}

bool c::value_type::operator == (const value_type& other) const {
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) != *reinterpret_cast<const uint64_t*>(&other[0])) return false;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) != *reinterpret_cast<const uint64_t*>(&other[8])) return false;
    if (*reinterpret_cast<const uint32_t*>(&(*this)[16]) != *reinterpret_cast<const uint32_t*>(&other[16])) return false;
    return true;
}

bool c::value_type::operator != (const value_type& other) const {
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) != *reinterpret_cast<const uint64_t*>(&other[0])) return true;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) != *reinterpret_cast<const uint64_t*>(&other[8])) return true;
    if (*reinterpret_cast<const uint32_t*>(&(*this)[16]) != *reinterpret_cast<const uint32_t*>(&other[16])) return true;
    return false;
}

bool c::value_type::operator < (const value_type& other) const {
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) < *reinterpret_cast<const uint64_t*>(&other[0])) return true;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) > *reinterpret_cast<const uint64_t*>(&other[0])) return false;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) < *reinterpret_cast<const uint64_t*>(&other[8])) return true;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) > *reinterpret_cast<const uint64_t*>(&other[8])) return false;
    if (*reinterpret_cast<const uint32_t*>(&(*this)[16]) < *reinterpret_cast<const uint32_t*>(&other[16])) return true;
    return false;
}

bool c::value_type::is_zero() const {
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) != 0) return false;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) != 0) return false;
    if (*reinterpret_cast<const uint32_t*>(&(*this)[16]) != 0) return false;
    return true;
}

bool c::value_type::is_not_zero() const {
    if (*reinterpret_cast<const uint64_t*>(&(*this)[0]) != 0) return true;
    if (*reinterpret_cast<const uint64_t*>(&(*this)[8]) != 0) return true;
    if (*reinterpret_cast<const uint32_t*>(&(*this)[16]) != 0) return true;
    return false;
}

void c::value_type::zero() {
    *reinterpret_cast<uint64_t*>(&(*this)[0]) = 0;
    *reinterpret_cast<uint64_t*>(&(*this)[8]) = 0;
    *reinterpret_cast<uint32_t*>(&(*this)[16]) = 0;
}

c::value_type c::digest(const vector<uint8_t>& buf) {
    c hasher;
    value_type h;
    hasher.write(&buf[0], buf.size());
    hasher.finalize(h);
    return h;
}

pair<ko, c::value_type> c::digest_file(const string& file) {
    vector<uint8_t> buf;
    auto r = io::read_file_(file, buf);
    if (is_ko(r)) {
        return make_pair(r, value_type(0));
    }
    return make_pair(ok, digest(buf));
}

c::value_type c::digest(const string& s) {
    if (s.empty()) return value_type(0);
    c hasher;
    value_type h;
    hasher.write(s);
    hasher.finalize(h);
    return h;
}

c::value_type& c::value_type::operator = (const string& b58) {
    set_b58(b58);
    return *this;
}

c::value_type& c::value_type::operator = (unsigned int i) {
    set(i);
    return *this;
}

