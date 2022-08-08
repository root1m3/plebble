// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2015-2017 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Why base-58 instead of standard base-64 encoding?
 * - Don't want 0OIl characters that look the same in some fonts and
 *      could be used to create visually identical looking data.
 * - A string with non-alphanumeric characters is not as easily accepted as input.
 * - E-mail usually won't line-break if there's no punctuation to break at.
 * - Double-clicking selects the whole string as one word if it's all alphanumeric.
 */
#pragma once
#include <string>
#include <vector>
#include <array>

namespace us::gov::crypto::b58 {

    using namespace std;

    string encode(const string&);
    string encode(const unsigned char* pbegin, const unsigned char* pend);
    string encode(const vector<unsigned char>&);
    string encode(const vector<uint32_t>&);

    string decode_string(const string& b58);
    vector<unsigned char> decode(const string& b58);
    bool decode(const string& b58, vector<unsigned char>& dest);
    bool decode(const char* psz, vector<unsigned char>&);
    bool decode(const char* psz, array<unsigned char, 32>&);
    bool decode(const char* psz, unsigned char* dest, size_t exact_length);

    string to_hex(const unsigned char* bin, size_t sz);
    vector<uint8_t> from_hex(const string& hex);
    string to_hex(const vector<uint8_t>& bin);

}

