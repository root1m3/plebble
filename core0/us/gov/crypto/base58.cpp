// Copyright (c) 2014-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>

#include <us/gov/likely.h>
#include <us/gov/config.h>

#define loglevel "crypto"
#define logclass "base58"
#include "logs.inc"

using namespace std;
using namespace us::gov::crypto;

namespace {

    static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    static const int8_t mapBase58[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
        -1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
        22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
        -1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
        47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    };

    constexpr inline bool is_space( char c ) noexcept {
        return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
    }

}

string b58::decode_string(const string& s) {
    vector<unsigned char> v;
    if (!decode(s.c_str(), v)) return "";
    v.push_back('\0');
    return string(reinterpret_cast<const char*>(v.data()));
}

vector<unsigned char> b58::decode(const string& s) {
    vector<unsigned char> v;
    if (!decode(s.c_str(), v)) return move(v);
    return v;
}

string b58::encode(const string& s) {
    assert(!s.empty());
    return encode(reinterpret_cast<const unsigned char*>(s.c_str()), reinterpret_cast<const unsigned char*>(s.c_str() + s.size()));
}

bool b58::decode( const char* psz, vector<unsigned char>& vch ) {
    while (*psz && is_space(*psz)) psz++;
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    int size = strlen(psz) * 733 /1000 + 1;
    vector<unsigned char> b256(size);
    while (*psz && !is_space(*psz)) {
        int carry = mapBase58[(uint8_t)*psz];
        if (carry == -1) return false;
        int i = 0;
        for (vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    while (is_space(*psz)) psz++;
    if (unlikely(*psz!=0)) return false;
    vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0) it++;
    vch.reserve(zeroes + (b256.end() - it));
    vch.assign(zeroes, 0x00);
    while (it != b256.end())
        vch.push_back(*(it++));
    return true;
}

bool b58::decode(const char* psz, unsigned char* dest, size_t exact_length) {
    while (*psz && is_space(*psz)) psz++;
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    int size = strlen(psz) * 733 /1000 + 1;
    vector<unsigned char> b256(size);
    while (*psz && !is_space(*psz)) {
        int carry = mapBase58[(uint8_t)*psz];
        if (carry == -1) return false;
        int i = 0;
        for (vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    while (is_space(*psz)) psz++;
    if (unlikely(*psz != 0)) return false;
    vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0) it++;
    if (exact_length != zeroes + (b256.end() - it)) return false;
    memset(dest, 0, zeroes);
    memcpy(dest + zeroes, &*it, b256.end() - it);
    return true;
}

bool b58::decode(const char* psz, array<unsigned char, 32>& vch) {
    while (*psz && is_space(*psz)) psz++;
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        psz++;
    }
    int size = strlen(psz) * 733 /1000 + 1;
    vector<unsigned char> b256(size);
    while (*psz && !is_space(*psz)) {
        int carry = mapBase58[(uint8_t)*psz];
        if (carry == -1) return false;
        int i = 0;
        for (vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        psz++;
    }
    while (is_space(*psz)) psz++;
    if (unlikely(*psz != 0)) return false;
    vector<unsigned char>::iterator it = b256.begin() + (size - length);
    while (it != b256.end() && *it == 0) it++;
    if ((zeroes + (b256.end() - it)) != 32) return false;
    int i = 0;
    for (; i < zeroes; ++i) vch[i]=0;
    while (it != b256.end()) vch[i++] = *(it++);
    return true;
}

string b58::encode(const unsigned char* pbegin, const unsigned char* pend) {
    // Skip & count leading zeroes.
    int zeroes = 0;
    int length = 0;
    while (pbegin != pend && *pbegin == 0) {
        pbegin++;
        zeroes++;
    }
    int size = (pend - pbegin) * 138 / 100 + 1;
    vector<unsigned char> b58(size);
    while (pbegin != pend) {
        int carry = *pbegin;
        int i = 0;
        for (vector<unsigned char>::reverse_iterator it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); it++, i++) {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }
        assert(carry == 0);
        length = i;
        pbegin++;
    }
    vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0) it++;
    string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while (it != b58.end()) str += pszBase58[*(it++)];
    return str;
}

string b58::encode(const vector<unsigned char>& v) {
    return encode(v.data(), v.data() + v.size());
}

string b58::encode(const vector<uint32_t>& v) {
    return encode((uint8_t*)v.data(), ((uint8_t*) v.data()) + (sizeof(uint32_t) * v.size()));
}

bool b58::decode(const string& str, vector<unsigned char>& v) {
    return decode(str.c_str(), v);
}

string b58::to_hex(const unsigned char* bin, size_t sz) {
    ostringstream os;

    for (int i = 0; i < sz; ++i) {
        unsigned char x = bin[i];
        x >>= 4;
        int g;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
        x = bin[i] & 0x0F;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
    }
    return os.str();
}

string b58::to_hex(const vector<uint8_t>& bin) {
    ostringstream os;
    for (int i = 0; i < bin.size(); ++i) {
        unsigned char x = bin[i];
        x >>= 4;
        int g;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
        x = bin[i] & 0x0F;
        if (x < 10) g = '0' + x; else g = 'A' + (x - 10);
        os << (char)g;
    }
    return os.str();
}

vector<uint8_t> b58::from_hex(const string& hex) {
    vector<uint8_t> k;
    if ((hex.size() & 1) != 0) {
        return k;
    }
    int j = 0;
    istringstream is(hex);
    int sz = hex.size() / 2;
    for (int i = 0; i < sz; i += 2) {
        char x = ::toupper(hex[i]);
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

