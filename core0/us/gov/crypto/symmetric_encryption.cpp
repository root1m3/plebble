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
#include "symmetric_encryption.h"

#include <iterator>

#include <us/gov/likely.h>

#include <secp256k1_ecdh.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>

#define loglevel "gov/crypto"
#define logclass "symmetric_encryption"
#include "logs.inc"

using namespace us::gov::crypto;
using c = us::gov::crypto::symmetric_encryption;
using us::ko;
using CryptoPP::BufferedTransformation;
using CryptoPP::AuthenticatedSymmetricCipher;
using CryptoPP::Redirector;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;


ko c::init(const keys::priv_t& sk, const keys::pub_t& pub_other) {
    log("init. pub_other:", pub_other);
    ko r = ec::instance.generate_shared_key(key_, sizeof(key_), sk, pub_other);
    if (unlikely(is_ko(r))) {
        log(r);
        return r;
    }
    return ok;
}

namespace {
    void log_ratio(size_t e, size_t d) {   //--strip
        log("IV sz ", c::iv_size,              //--strip
            "scrambled msg sz", e,         //--strip
            "enc/dec ratio(", d, ") = ", (double)e / (double)d);    //--strip
    }                                         //--strip
}

ko c::encrypt(const vector<unsigned char>& cleartext, vector<unsigned char>& ciphertext, size_t ciphertext_offset) {
    log("encrypt block of", cleartext.size(), "bytes.");
    size_t estimated_sz = ciphertext_offset + iv_size + cleartext.size() + AES::BLOCKSIZE;
    log("estimated size", estimated_sz);
    ciphertext.resize(estimated_sz);
    try {
        unsigned char iv_[iv_size];
        prng_.GenerateBlock(iv_, iv_size);
        GCM<AES>::Encryption enc;
        enc.SetKeyWithIV(key_, key_size, iv_, iv_size);
        ArraySink sink(ciphertext.data() + ciphertext_offset, ciphertext.size() - ciphertext_offset);
        ArraySource(cleartext.data(), cleartext.size(), true, new AuthenticatedEncryptionFilter(enc, new Redirector(sink), false, tag_size));
        size_t final_sz = ciphertext_offset + sink.TotalPutLength() + iv_size;
        log("final size", final_sz);
        ciphertext.resize(final_sz);
        assert(estimated_sz >= final_sz); //if fails here adjust encmsg_max_sz
        log_ratio(ciphertext.size() - ciphertext_offset, cleartext.size()); //--strip
        memcpy(ciphertext.data() + ciphertext_offset + sink.TotalPutLength(), iv_, iv_size);
        log("total encrypted payload", ciphertext.size() - ciphertext_offset, "bytes.");
    }
    catch(CryptoPP::InvalidArgument& e) {
        auto r = "KO 30921 Invalid argument in encryption.";
        log(r, e.what());
        return r;
    }
    return ok;
}

ko c::decrypt(const vector<unsigned char>& ciphertext, vector<unsigned char>& cleartext) {
    return decrypt(ciphertext.data(), ciphertext.size(), cleartext);
}

ko c::decrypt(const unsigned char* ciphertext, size_t ciphertext_sz, vector<unsigned char>& cleartext) {
    if (unlikely(ciphertext_sz < iv_size)) {
        auto r = "KO 44031 Message too small to decrypt.";
        log(r);
        return r;
    }
    size_t sz_msg = ciphertext_sz - iv_size;
    log("decrypt block of ", ciphertext_sz, "bytes. msg sz=", sz_msg, "+ IV size", iv_size);
    cleartext.resize(ciphertext_sz);
    try {
        GCM<AES>::Decryption dec;
        dec.SetKeyWithIV(key_, key_size, &ciphertext[sz_msg], iv_size);
        ArraySink sink(cleartext.data(), cleartext.size());
        AuthenticatedDecryptionFilter filter(dec, new Redirector(sink), AuthenticatedDecryptionFilter::DEFAULT_FLAGS, tag_size);
        ArraySource(ciphertext, sz_msg, true, new Redirector(filter));
        cleartext.resize(sink.TotalPutLength());
        log_ratio(ciphertext_sz, cleartext.size()); //--strip
    }
    catch (const CryptoPP::HashVerificationFilter::HashVerificationFailed& e) {
        ko r = "KO 44032 CryptoPP::HashVerificationFilter::HashVerificationFailed.";
        log(r, e.what());
        return r;
    }
    catch (const CryptoPP::InvalidArgument& e) {
        auto r = "KO 44033 Invalid decrypting argument.";
        log(r, e.what());
        return r;
    }
    catch (const CryptoPP::Exception& e) {
        auto r = "KO 44034 Decryption was unsuccessful.";
        log(r, e.what());
        return r;
    }
    return ok;
}

