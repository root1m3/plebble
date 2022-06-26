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
#include "ec.h"

#include <sstream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <fstream>

#include <secp256k1_ecdh.h>

#include "base58.h"

#define loglevel "gov/crypto"
#define logclass "ec"
#include "logs.inc"

using namespace us::gov::crypto;
using c = us::gov::crypto::ec;
using us::ko;

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

c c::instance;

const char* c::KO_96857 = "KO 96857 Invalid private key.";

static void secperr_callback(const char* str, void* data) {
    cerr << "libsecp256k1: " << str << endl;
    #ifdef DEBUG
        abort();
    #endif
}

c::ec() {
    ctx = secp256k1_context_create( SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY );
    if (ctx == 0) {
        cerr << "Could not initialize EC context." << endl;
        exit(1);
    }
    secp256k1_context_set_illegal_callback(ctx, secperr_callback, 0);
    static_assert( sigmsg_hasher_t::output_size > hasher_t::output_size );
}

c::~ec() {
    secp256k1_context_destroy(ctx);
}

int c::keys::priv_t::reset_if_distinct(const priv_t& sk) {
    if (memcmp(data(), &sk, priv_t::ser_size) == 0) return 0;
    memcpy(data(), &sk[0], priv_t::ser_size);
    if (is_ko(verifyx())) return -1;
    return 1;
}

int c::keys::reset_if_distinct(const priv_t& sk) {
    auto r = priv.reset_if_distinct(sk);
    if (r == 0) return 0;
    if (r == -1) {
        pub.zero();
        return -1;
    }
    if (!secp256k1_ec_pubkey_create(ec::instance.ctx, &pub, sk.data())) {
        log("Cannot create public key from priv");
        priv.zero();
        pub.zero();
        return -1;
    }
    log("reseted keys", pub);
    pub.set_valid();
    return 1;
}

void c::keys::reset() {
    priv_t sk;
    reset(sk);
}

void c::keys::priv_t::reset(const priv_t& sk) {
    memcpy(data(), sk.data(), ser_size);
}

void c::keys::priv_t::reset(priv_t&& sk) {
    memcpy(data(), sk.data(), ser_size);
}

void c::keys::priv_t::write(unsigned char* out) const {
    memcpy(out, data(), ser_size);
}

void c::keys::priv_t::read(const unsigned char* in) {
    memcpy(data(), in, ser_size);
}

void c::keys::reset(const priv_t& sk) {
    priv.reset(sk);
    if (!secp256k1_ec_pubkey_create(ec::instance.ctx, &pub, sk.data())) {
        log("Cannot create public key from priv");
        pub.zero();
    }
    else {
        pub.set_valid();
    }
}

void c::keys::reset(priv_t&& sk) {
    priv.reset(move(sk));
    if (!secp256k1_ec_pubkey_create(ec::instance.ctx, &pub, sk.data())) {
        log("Cannot create public key from priv");
        pub.zero();
    }
    else {
        pub.set_valid();
    }
}

c::keys::keys(const priv_t& sk) {
    reset(sk);
}

c::keys::keys(priv_t&& sk) {
    reset(move(sk));
}

c::keys::keys(const keys& other): pub(other.pub) {
    memcpy(&priv[0], &other.priv[0], priv_t::ser_size);
}

c::keys::keys(keys&& other): pub(move(other.pub)) {
    memcpy(&priv[0], &other.priv[0], priv_t::ser_size);
}

c::keys::pub_t::pub_t(): valid(false), h(false) {
}

c::keys::pub_t::pub_t(const string& b58) {
    read_b58(b58);
}

c::keys::pub_t::pub_t(const vector<unsigned char>& ser) {
    read_ser(ser);
}

const c::keys::pub_t::hash_t& c::keys::pub_t::hash() const {
    if (unlikely(!h)) {
        hash_cached = compute_hash();
        h = true;
    }
    return hash_cached;
}

c::keys::pub_t::hash_t c::keys::pub_t::compute_hash() const {
    if (unlikely(!valid)) return hash_t(0);
    unsigned char out[ser_size];
    size_t len = ser_size;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED))) {
        #if CFG_COUNTERS == 1
            ++counters.unable_serialize_pubkey;
        #endif
        return hash_t(0);
    }
    hasher_t hasher;
    hasher.write(&out[0], ser_size);
    hash_t v;
    hasher.finalize(v);
    return move(v);
}

void c::keys::priv_t::zero() {
    memset(data(), 0, ser_size);
    assert(is_zero());
}

c::keys::priv_t::priv_t() {
    zero();
}

c::keys::priv_t::priv_t(const priv_t& other) {
    reset(other);
}

c::keys::priv_t::priv_t(priv_t&& other) {
    reset(move(other));
}

c::keys::priv_t::priv_t(const string& b58) {
    if (unlikely(!set_b58(b58))) zero();
}

c::keys::priv_t::priv_t(const char* b58) {
    if (unlikely(!set_b58(b58))) zero();
}

c::keys::priv_t c::keys::generate_priv() {
    priv_t o;
    o.generate();
    return move(o);
}

bool c::keys::priv_t::is_zero() const {
    auto d = data();
    auto e = d + size();
    for (; d < e; d += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != 0) {
            return false;
        }
    }
    return true;
}

bool c::keys::priv_t::is_not_zero() const {
    auto d = data();
    auto e = d + size();
    for (; d < e; d += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != 0) {
            return true;
        }
    }
    return false;
}

string c::keys::priv_t::to_b58() const {
    return b58::encode(&*begin(), &*end());
}

bool c::keys::pub_t::to_vector(vector<unsigned char>& out) const {
    log("to_vector");
    if (unlikely(!valid)) {
        log("KO 5502");
        out.clear();
        return false;
    }
    out.resize(ser_size);
    size_t len = ser_size;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out.data(), &len, this, SECP256K1_EC_COMPRESSED))) {
        #if CFG_COUNTERS == 1
            ++counters.unable_serialize_pubkey;
        #endif
        log("KO 7060");
        out.clear();
        return false;
    }
    log("pubkey vector length", out.size());
    return true;
}

void c::keys::pub_t::write(unsigned char* out) const {
    log("pub_t::write");
    if (unlikely(!valid)) {
        memset(out, 0, ser_size);
        return;
    }
    size_t len = ser_size;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED))) {
        #if CFG_COUNTERS == 1
            ++counters.unable_serialize_pubkey;
        #endif
        log("KO 7060");
        memset(out, 0, ser_size);
        return;
    }
}

void c::keys::pub_t::read(const unsigned char* p) {
    if (unlikely(!secp256k1_ec_pubkey_parse(ec::instance.ctx, this, p, ser_size))) {
        zero();
        return;
    }
    set_valid();
}

size_t c::keys::pub_t::ser_sz() const {
    return ser_size;
}

unsigned char* c::keys::pub_t::write_to(unsigned char* p) const {
    if (likely(valid)) {
        size_t len = ser_size;
        secp256k1_ec_pubkey_serialize(ec::instance.ctx, p, &len, this, SECP256K1_EC_COMPRESSED);
    }
    else {
        memset(p, 0, ser_size);
    }
    return p + ser_size;
}

const unsigned char* c::keys::pub_t::read_from(const unsigned char* p, const unsigned char* bend) {
    if (p + ser_size > bend) return nullptr;
    if (unlikely(!secp256k1_ec_pubkey_parse(ec::instance.ctx, this, p, ser_size))) {
        zero();
        return p + ser_size;
    }
    else {
        set_valid();
    }
    return p + ser_size;
}

string c::keys::pub_t::to_b58() const {
    if (valid) {
        vector<unsigned char> out;
        to_vector(out);
        return b58::encode(out.data(), out.data() + out.size());
    }
    else {
        return "-";
    }
}

c::keys::priv_t c::keys::priv_t::from_b58(const string& s) {
    priv_t k;
    if (!b58::decode(s.c_str(), k)) {
        k.zero();
    }
    return move(k);
}

bool c::keys::priv_t::set_b58(const string& s) {
    return b58::decode(s.c_str(), *this);
}

void c::keys::pub_t::read_ser(const vector<unsigned char>& ser) {
    if (ser.size() != ser_size) {
        #if CFG_COUNTERS == 1
            ++counters.invalid_b58_pubkey;
        #endif
        zero();
        return;
    }
    if (unlikely(!secp256k1_ec_pubkey_parse( ec::instance.ctx, this, ser.data(), ser.size()))) {
        #if CFG_COUNTERS == 1
            ++counters.invalid_ser_pubkey;
        #endif
        log("KO 3009 Reading public key.");
        zero();
        return;
    }
    set_valid();
}

void c::keys::pub_t::read_b58(const string& s) {
    vector<unsigned char> v;
    v.reserve(ser_size);
    if (s == "-") {
        zero();
        return;
    }
    if (!b58::decode(s, v)) {
        #if CFG_COUNTERS == 1
            ++counters.invalid_b58_pubkey;
        #endif
        zero();
        return;
    }
    if (v.size() != ser_size) {
        #if CFG_COUNTERS == 1
            ++counters.invalid_b58_pubkey;
        #endif
        zero();
        return;
    }
    read_ser(v);
}

c::keys::pub_t c::keys::pub_t::from_b58(const string& b58) {
    return pub_t(b58);
}

c::keys::priv_t& c::keys::priv_t::operator = (const string& b58) {
    set_b58(b58);
    return *this;
}

c::keys::priv_t& c::keys::priv_t::operator = (const priv_t& other) {
    reset(other);
    return *this;
}

c::keys::priv_t& c::keys::priv_t::operator = (priv_t&& other) {
    reset(move(other));
    return *this;
}

bool c::keys::pub_t::set_b58(const string& b58) {
    read_b58(b58);
    return valid;
}

bool c::keys::pub_t::operator == (const keys::pub_t& other) const {
    if (valid != other.valid) return false;
    if (!valid) return true;
    auto d = &data[0];
    auto e = d + mem_size;
    auto o = &other.data[0];
    for (; d < e; d += sizeof(uint64_t), o += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != *reinterpret_cast<const uint64_t*>(o)) return false;
    }
    return true;
}

bool c::keys::pub_t::operator != ( const keys::pub_t& other) const {
    if (valid != other.valid) return true;
    if (!valid) return false;
    auto d = &data[0];
    auto e = d + mem_size;
    auto o = &other.data[0];
    for (; d < e; d += sizeof(uint64_t), o += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != *reinterpret_cast<const uint64_t*>(o)) return true;
    }
    return false;
}

bool c::keys::pub_t::operator == (const string& b58) const {
    pub_t p(b58);
    return *this == p;
}

bool c::keys::pub_t::operator != (const string& b58) const {
    pub_t p(b58);
    return *this != p;
}

bool c::keys::priv_t::operator == (const string& b58) const {
    priv_t p(b58);
    return *this == p;
}

bool c::keys::priv_t::operator != (const string& b58) const {
    priv_t p(b58);
    return *this != p;
}


bool c::keys::priv_t::operator == (const keys::priv_t& other) const {
    auto d = data();
    auto e = d + ser_size;
    auto o = other.data();
    for (; d < e; d += sizeof(uint64_t), o += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != *reinterpret_cast<const uint64_t*>(o)) return false;
    }
    return true;
}

bool c::keys::priv_t::operator != (const keys::priv_t& other) const {
    auto d = data();
    auto e = d + ser_size;
    auto o = other.data();
    for (; d < e; d += sizeof(uint64_t), o += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != *reinterpret_cast<const uint64_t*>(o)) return true;
    }
    return false;
}

c::keys::pub_t::pub_t(const pub_t& other): h(other.h), hash_cached(other.hash_cached), valid(other.valid)  {
    memcpy(data, other.data, mem_size);
}

c::keys::pub_t::pub_t(pub_t&& other): h(move(other.h)), hash_cached(move(other.hash_cached)), valid(move(other.valid))  {
    memcpy(data, other.data, mem_size);
}

c::keys::pub_t& c::keys::pub_t::operator = (const string& b58) {
    read_b58(b58);
    return *this;
}

c::keys::pub_t& c::keys::pub_t::operator = (const keys::pub_t& other) {
    memcpy(data, other.data, mem_size);
    valid = other.valid;
    hash_cached = other.hash_cached;
    h = other.h;
    return *this;
}

c::keys::pub_t& c::keys::pub_t::operator = (keys::pub_t&& other) {
    memcpy(data, other.data, mem_size);
    valid = move(other.valid);
    hash_cached = move(other.hash_cached);
    h = move(other.h);
    return *this;
}

vector<uint8_t> c::sig_encode_der(const sig_t& sig) const {
    vector<uint8_t> der;
    size_t nSigLen{72};
    der.resize(nSigLen);
    if (unlikely(!secp256k1_ecdsa_signature_serialize_der(ctx, (unsigned char*)der.data(), &nSigLen, &sig))) {
        ko r = "KO 30291 Couldn't serialize signature.";
        log(r);
        der.clear();
        return der;
    }
    der.resize(nSigLen);
    return move(der);
}

string c::sig_encode_der_b58(const sig_t& sig) const {
    return b58::encode(sig_encode_der(sig));
}

void c::sig_t::write(unsigned char* out) const {
    memcpy(out, &data[0], mem_size);
}

void c::sig_t::read(const unsigned char* in) {
    memcpy(&data[0], in, mem_size);
}

string c::sig_t::to_b58() const {
    return b58::encode(&data[0], &data[0] + mem_size);
}

bool c::sig_t::set_b58(const string& s) {
    return b58::decode(s.c_str(), &data[0], mem_size);
}

void c::sig_t::zero() {
    memset(&data[0], 0, mem_size);
    assert(is_zero());
}

bool c::sig_t::is_zero() const {
    auto d = &data[0];
    auto e = d + mem_size;
    for (; d < e; d += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != 0) {
            return false;
        }
    }
    return true;
}

bool c::sig_t::is_not_zero() const {
    auto d = &data[0];
    auto e = d + mem_size;
    for (; d < e; d += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != 0) {
            return true;
        }
    }
    return false;
}

ko c::sign(const keys::priv_t& pk, const vector<unsigned char>& msg, sig_t& signature) const {
    sigmsg_hasher_t hasher;
    sigmsg_hasher_t::value_type hash;
    hasher.write(msg.data(), msg.size());
    hasher.finalize(hash);
    return sign(pk, hash, signature);
}

ko c::sign(const keys::priv_t& pk, const hash_t& msg, sig_t& signature) const {
    sigmsg_hasher_t::value_type msghash;
    memcpy(&msghash[0], &msg[0], hasher_t::output_size);
    memset(&msghash[hasher_t::output_size], 0, sigmsg_hasher_t::output_size - hasher_t::output_size);
    return sign(pk, msghash, signature);
}

ko c::sign(const keys::priv_t& pk, const string& text, sig_t& signature) const {
    log("signing string msg", text);
    sigmsg_hasher_t hasher;
    sigmsg_hasher_t::value_type hash;
    hasher.write( (const unsigned char*) text.c_str(), text.size() );
    hasher.finalize(hash);
    return sign(pk, hash, signature);
}

ko c::sign(const keys::priv_t& pk, const sigmsg_hasher_t::value_type& msgh, sig_t& sig) const {
    log("signing msgh", msgh, "with sk for pubkey", keys::get_pubkey(pk));
    if (unlikely(!secp256k1_ecdsa_sign(ctx, &sig, &msgh[0], &pk[0], secp256k1_nonce_function_rfc6979, 0))) {
        #if CFG_COUNTERS == 1
            ++counters.error_signing;
        #endif
        ko r = "KO 59451 secp error.";
        log(r);
        sig.zero();
        return r;
    }
    #if CFG_COUNTERS == 1
        ++counters.signatures_created;
    #endif
    #ifdef DEBUG
        { //assert secp256k1 produces normalized signatures (low-s)
        sig_t nsig;
        int was_normalized = secp256k1_ecdsa_signature_normalize(ctx, &nsig, &sig);
        log("was normalized", was_normalized == 0);
        assert(was_normalized == 0);
        }
    #endif
    log("sign output", sig.to_b58());
    return ok;
}

ko c::keys::verifyx() const {
    return verifyx(priv);
}

ko c::generate_shared_key(unsigned char* shared_key, size_t size, const keys::priv_t& priv, const keys::pub_t& pub) {
    unsigned char temp_key[32];
    if (unlikely(!pub.valid)) {
        return "KO 22012 Invalid public key.";
    }
    auto r = keys::verifyx(priv);
    if (unlikely(is_ko(r))) {
        return r;
    }
    if (unlikely(size > 32)) {
        auto r = "KO 22015 key too big.";
        log(r);
        return r;
    }
    if(unlikely(!secp256k1_ecdh(ctx, temp_key, &pub, &priv[0], 0, 0))) {
        #if CFG_COUNTERS==1
            ++counters.error_creating_shared_secret;
        #endif
        auto r = "KO 22014 Error creating shared secret.";
        log(r);
        return r;
    }
    memcpy(shared_key, temp_key, size);
    return ok;
}

bool c::verify(const keys::pub_t& pk, const string& text, const sig_t& sig) const {
    sigmsg_hasher_t hasher;
    hasher.write((const unsigned char*)text.c_str(), text.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify(pk, hash, sig);
}

bool c::verify(const keys::pub_t& pk, const vector<unsigned char>& msg, const sig_t& sig) const {
    sigmsg_hasher_t hasher;
    hasher.write(msg.data(), msg.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify(pk, hash, sig);
}

bool c::verify(const keys::pub_t& pk, const hash_t& msg, const sig_t& sig) const {
    sigmsg_hasher_t::value_type msghash;
    memcpy(&msghash[0], &msg[0], hasher_t::output_size);
    memset(&msghash[hasher_t::output_size], 0, sigmsg_hasher_t::output_size - hasher_t::output_size);
    return verify(pk, msghash, sig);
}

bool c::verify(const keys::pub_t& pk, const sigmsg_hasher_t::value_type& msgh, const sig_t& sig) const {
    ostream nullos(0);
    return verify(pk, msgh, sig, nullos);
}

bool c::verify(const keys::pub_t& pk, const sigmsg_hasher_t::value_type& msgh, const sig_t& sig, ostream& err) const {
    log("verify input", pk, msgh, sig.to_b58());
    if (unlikely(sig.is_zero())) {
        auto r = "KO 02101 empty signature.";
        err << r << '\n';
        log(r);
        return false;
    }
    bool r = secp256k1_ecdsa_verify(ec::instance.ctx, &sig, &msgh[0], &pk) == 1;
    if (likely(r)) {
        #if CFG_COUNTERS == 1
            ++counters.signature_verification_pass;
        #endif
    }
    else {
        log("secp256k1_ecdsa_verify", "TAMPERING");
        err << "pubkey: " << pk << '\n';
        err << "msg hash: " << msgh << '\n';
        err << "sig: " << sig.to_b58() << '\n';
        err << "KO 0210.4 secp256k1_ecdsa_verify. Incorrect signature.\n";
        #if CFG_COUNTERS == 1
            ++counters.signature_verification_fail;
        #endif
    }
    return r;
}

bool c::verify_not_normalized(const keys::pub_t& pk, const sigmsg_hasher_t::value_type& msgh, const sig_t& sig) const {
    log("verify_not_normalized", "msgh", msgh, "pubk", pk, "sig-der-b58", sig.to_b58());
    sig_t nsig;
    int was_normalized = secp256k1_ecdsa_signature_normalize(ctx, &nsig, &sig);
    log("was normalized", was_normalized == 0);
    bool r = secp256k1_ecdsa_verify(ec::instance.ctx, &nsig, &msgh[0], &pk) == 1;
    #if CFG_COUNTERS == 1
        if (likely(r)) {
            ++counters.signature_verification_pass;
        }
        else {
            log("KO 40320 failed EC verification.", "msgh", msgh, "pubk", pk, "sig", sig.to_b58());
            ++counters.signature_verification_fail;
        }
    #else                                       //--strip
        if (unlikely(!r)) {                     //--strip
            log("KO 10250 failed EC verification.", "msgh", msgh, "pubk", pk, "sig", sig.to_b58()); //--strip
        }                                       //--strip
    #endif
    return r;
}

bool c::verify_not_normalized(const keys::pub_t& pk, const string& text, const sig_t& sig) const {
    log("verifying string message sz", text.size(), "against pubkey", pk, "sig_der", sig.to_b58());
    sigmsg_hasher_t hasher;
    hasher.write((const unsigned char*) text.c_str(), text.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify_not_normalized(pk, hash, sig);
}

bool c::verify_not_normalized(const keys::pub_t& pk, const vector<unsigned char>& msg, const sig_t& sig) const {
    sigmsg_hasher_t hasher;
    hasher.write(msg.data(), msg.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify_not_normalized(pk, hash, sig);
}

ko c::keys::verifyx(const keys::priv_t& k) {
    auto r = !k.is_zero() && secp256k1_ec_seckey_verify( ec::instance.ctx, &k[0] ) == 1;
    #if CFG_COUNTERS == 1
        if (likely(r)) {
            ++counters.privkey_verification_pass;
        }
        else {
            ++counters.privkey_verification_fail;
        }
    #endif
    if (!r) {
        log(KO_96857);
        return KO_96857;
    }
    return ok;
}

c::keys::pub_t c::keys::get_pubkey(const priv_t& privk) {
    pub_t k;
    if(unlikely(!secp256k1_ec_pubkey_create(ec::instance.ctx, &k, privk.data()))) {
        #if CFG_COUNTERS == 1
            ++counters.error_priv_2_pub;
        #endif
        k.zero();
        return k;
    }
    k.set_valid();
    return move(k);
}

ko c::keys::priv_t::verifyx() const {
    auto r = !is_zero() && secp256k1_ec_seckey_verify(ec::instance.ctx, data()) == 1;
    #if CFG_COUNTERS == 1
        if (likely(r)) {
            ++counters.privkey_verification_pass;
        }
        else {
            ++counters.privkey_verification_fail;
        }
    #endif
    if (unlikely(!r)) {
        auto e = "KO 60599 Secret Key doesn't verify.";
        log(e);
        return e;
    }
    return ok;
}

ko c::keys::priv_t::generate() {
    ifstream f("/dev/urandom");
    if (!f.good()) {
        auto r = "KO 73021 Opening entropy file /dev/urandom";
        log(r);
        zero();
        return r;
    }
    f.read(reinterpret_cast<char*>(data()), c::keys::priv_mem_size);
    auto r = verifyx();
    if (is_ko(r)) {
        zero();
    }
    return r;
}

c::keys::pub_t c::keys::priv_t::pub() const {
    pub_t k;
    if(unlikely(!secp256k1_ec_pubkey_create(ec::instance.ctx, &k, data()))) {
        #if CFG_COUNTERS == 1
            ++counters.error_priv_2_pub;
        #endif
        k.zero();
        return k;
    }
    k.valid = true;
    return move(k);
}

c::keys c::keys::generate() {
    keys k;
    auto r = k.priv.generate();
    if (is_ko(r)) {
        cerr << r << endl;
        exit(1);
    }
    k.pub = get_pubkey(k.priv);
    return move(k);
}

void c::sig_t::from_b58(const string& b58) {
    if (!b58::decode(b58.c_str(), &data[0], sig_t::mem_size)) {
        zero();
    }
}

c::sig_t c::sig_from_der(const sig_der_t& der) const {
    log("sig_from_der. sz", der.size());
    sig_t sig;
    if (unlikely(!secp256k1_ecdsa_signature_parse_der(ctx, &sig, der.data(), der.size()))) {
        #if CFG_COUNTERS == 1
            ++counters.signature_verification_der_parse_error;
        #endif
        log("KO 20100 Invalid DER signature.");
        sig.zero();
        return move(sig);
    }
    return move(sig);
}

c::sig_t c::sig_from_der(const string& der_b58) const {
    auto v = b58::decode(der_b58);
    if (v.empty()) {
        log("KO 28100 Invalid b58 encoding.");
        sig_t sig;
        return sig;
    }
    return sig_from_der(v);
}

c::sig_t c::sig_from_b58_prev(const string& b58) const {
    log("sig_from_b58_prev", b58);
    sig_t sig;
    vector<uint8_t> der;
    if (!b58::decode(b58.c_str(), der)) {
        log("KO 67752 cannnode decode der signature");
        sig.zero();
        return move(sig);
    }
    return sig_from_der(der);
}

c::sig_t c::sig_from_b58(const string& b58) {
    sig_t v;
    v.from_b58(b58);
    return move(v);
}

bool c::sig_t::operator == (const sig_t& other) const {
    auto d = &data[0];
    auto e = d + mem_size;
    auto o = &other.data[0];
    for (; d < e; d += sizeof(uint64_t), o += sizeof(uint64_t)) {
        if (*reinterpret_cast<const uint64_t*>(d) != *reinterpret_cast<const uint64_t*>(o)) return false;
    }
    return true;
}

#if CFG_COUNTERS == 1
    void c::counters_t::dump(ostream& os) const {
        os << "unable_serialize_pubkey " << unable_serialize_pubkey << '\n';
        os << "invalid_b58_pubkey " << invalid_b58_pubkey << '\n';
        os << "invalid_ser_pubkey " << invalid_ser_pubkey << '\n';
        os << "error_signing " << error_signing << '\n';
        os << "error_serializing_signature " << error_serializing_signature << '\n';
        os << "signatures_created " << signatures_created << '\n';
        os << "error_creating_shared_secret " << error_creating_shared_secret << '\n';
        os << "signature_verification_der_parse_error " << signature_verification_der_parse_error << '\n';
        os << "signature_verification_pass " << signature_verification_pass << '\n';
        os << "signature_verification_fail " << signature_verification_fail << '\n';
        os << "privkey_verification_pass " << privkey_verification_pass << '\n';
        os << "privkey_verification_fail " << privkey_verification_fail << '\n';
        os << "error_priv_2_pub " << error_priv_2_pub << '\n';
    }
#endif

size_t std::hash<us::gov::crypto::ec::keys::pub_t>::operator() (const us::gov::crypto::ec::keys::pub_t& g) const {
    return *reinterpret_cast<const size_t*>(g.hash().data());
}

istream& us::gov::crypto::operator >> (istream& is, ec::keys::pub_t& k) {
    string s;
    is >> s;
    k.set_b58(s);
    return is;
}

