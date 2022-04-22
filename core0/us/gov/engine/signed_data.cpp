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
#include "signed_data.h"
#include <us/gov/crypto/base58.h>
#include <cassert>
#include <sstream>

#define loglevel "gov/engine"
#define logclass "signed_data"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::signed_data0;
using us::ko;

void c::sign(const keys& k, const sigmsg_hasher_t::value_type& hash) {
    pubkey = k.pub;
    crypto::ec::instance.sign(k.priv, hash, signature);
    log("sign with pubkey", k.pub, "msghash", hash, "produced signature", signature.to_b58());
}

bool c::verify(const sigmsg_hasher_t::value_type& hash) const {
    ostream os(0);
    return verify(hash, os);
}

bool c::verify(const sigmsg_hasher_t::value_type& hash, ostream& os) const {
    log("verify", hash);
    auto r = crypto::ec::instance.verify(pubkey, hash, signature, os);
    log( "Verify", "pubkey", pubkey, "msghash", hash, "signature", signature.to_b58(), "output", r );
    return r;
}

void c::write_pretty(const string& lang, ostream& os) const {
    if (lang == "es") {
        write_pretty_es(os);
    }
    else {
        write_pretty_en(os);
    }
}

void c::write_pretty_en(ostream& os) const {
    os << "Signed by: " << pubkey.hash() << '\n';
    os << "Public key: " << pubkey << '\n';
    os << "Signature: " << signature.to_b58() << '\n';
}

void c::write_pretty_es(ostream& os) const {
    os << "Firmado por: " << pubkey.hash() << '\n';
    os << "Clave publica: " << pubkey << '\n';
    os << "Firma digital: " << signature.to_b58() << '\n';
}

using c1 = us::gov::engine::signed_data;

bool c1::verify(ostream& os) const {
    log("verify");
    sigmsg_hasher_t hasher;
    hash_data_to_sign(hasher);
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return b::verify(hash, os);
}

bool c1::verify() const {
    log("verify");
    sigmsg_hasher_t hasher;
    hash_data_to_sign(hasher);
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return b::verify(hash);
}

void c1::sign(const keys& k) {
    sigmsg_hasher_t hasher;
    hash_data_to_sign(hasher);
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    b::sign(k, hash);
}

void c1::sign(const privkey_t& sk) {
    keys k(sk);
    log("signing with pubkey", k.pub);
    sign(k);
}

us::gov::crypto::ripemd160::value_type c1::hash() const {
    ripemd160 hasher;
    ripemd160::value_type hash;
    hash_data(hasher);
    hasher.finalize(hash);
    return hash;
}

void c1::write_pretty_en(ostream& os) const {
    b::write_pretty_en(os);
    ostringstream vos;
    if (verify(vos)) {
        os << "Cryptographic verification SUCCEED. Data is legitimate.\n";
    }
    else {
        os << "WARNING: Cryptographic verification FAILED. Data is NOT legitimate.\n";
        os << vos.str() << '\n';
    }
}

void c1::write_pretty_es(ostream& os) const {
    b::write_pretty_es(os);
    ostringstream vos;
    if (verify(vos)) {
        os << "Verificacion criptografica correcta. Los datos son legitimos.\n";
    }
    else {
        os << "ATENCION: La verificacion criptografica no es correcta. Los datos NO son legitimos.\n";
        os << vos.str() << '\n';
    }
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(pubkey) + blob_writer_t::blob_size(signature);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(pubkey);
    writer.write(signature);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(pubkey);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(signature);
        if (is_ko(r)) return r;
    }
    return ok;
}

