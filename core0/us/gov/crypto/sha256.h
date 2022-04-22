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
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <cstring>
#include <array>
#include <us/gov/likely.h>
#include "ripemd160.h"

namespace us { namespace gov { namespace crypto {

    using namespace std;

    class sha256 {
    public:
        static constexpr uint32_t output_size = 32;
        sha256();

        struct value_type final: array<uint8_t, output_size> {
            using hasher_t = sha256;

            value_type() {}
            explicit value_type(int) { zero(); }
            bool operator < (const value_type&) const; //result depends on endianness (different results in different archs), ok for local hash tables
            bool is_zero() const;
            bool is_not_zero() const;
            void zero();
            string to_b58() const;
            ko from_b58(const string&);
        };

        void write(const uint8_t* data, size_t len);
        void write(const vector<uint8_t>&);
        void write(const string&);
        void write(const ripemd160::value_type&);
        void write(const value_type&);
        void write(const uint64_t&);
        void write(const int64_t&);
        void write(const uint32_t&);
        void write(const int32_t&);
        void write(bool);
        void write(const double&);
        void write(const uint8_t&);

        void finalize(unsigned char hash[output_size]);
        void finalize(value_type&);
        void reset();

    private:
        void initialize(uint32_t* s);
        uint32_t inline Ch(uint32_t x, uint32_t y, uint32_t z) { return z ^ (x & (y ^ z)); }
        uint32_t inline Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (z & (x | y)); }
        uint32_t inline Sigma0(uint32_t x) { return (x >> 2 | x << 30) ^ (x >> 13 | x << 19) ^ (x >> 22 | x << 10); }
        uint32_t inline Sigma1(uint32_t x) { return (x >> 6 | x << 26) ^ (x >> 11 | x << 21) ^ (x >> 25 | x << 7); }
        uint32_t inline sigma0(uint32_t x) { return (x >> 7 | x << 25) ^ (x >> 18 | x << 14) ^ (x >> 3); }
        uint32_t inline sigma1(uint32_t x) { return (x >> 17 | x << 15) ^ (x >> 19 | x << 13) ^ (x >> 10); }
        void inline Round(uint32_t a, uint32_t b, uint32_t c, uint32_t& d, uint32_t e, uint32_t f, uint32_t g, uint32_t& h, uint32_t k, uint32_t w) {
            uint32_t t1 = h + Sigma1(e) + Ch(e, f, g) + k + w;
            uint32_t t2 = Sigma0(a) + Maj(a, b, c);
            d += t1;
            h = t1 + t2;
        }
        void transform(uint32_t* s, const unsigned char* chunk);

    private:
        uint32_t s[8];
        unsigned char buf[64];
        uint64_t bytes;
    };

    inline ostream& operator << (ostream& os, const us::gov::crypto::sha256::value_type& v) {
        os << v.to_b58();
        return os;
    }

    inline istream& operator >> (istream& is, us::gov::crypto::sha256::value_type& v) {
        string s;
        is >> s;
        v.from_b58(s);
        return is;
    }

}}}

namespace std {

  template <>
  struct hash<us::gov::crypto::sha256::value_type> {
    size_t operator()(const us::gov::crypto::sha256::value_type& k) const {
        return *reinterpret_cast<const size_t*>(&k[0]);

    }
  };

}

