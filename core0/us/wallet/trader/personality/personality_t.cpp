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
#include <cassert>
#include <sstream>

#include <us/gov/crypto/base58.h>
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>

#include "personality_t.h"

#define loglevel "wallet/trader/personality"
#define logclass "personality_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet::trader::personality;
using c = us::wallet::trader::personality::personality_t;
using us::ko;

c::personality_t() {
    set_anonymous();
}

c::personality_t(const priv_t& sk, const moniker_t& moniker) : k(sk), moniker(moniker) {
    if (k.pub.valid) {
        id = k.pub.hash();
    }
    else {
        id = hash_t(0);
    }
}

c::personality_t::personality_t(const personality_t& other): k(other.k), moniker(other.moniker) {
    if (k.pub.valid) {
        id = k.pub.hash();
    }
    else {
        id = hash_t(0);
    }
}

c& c::operator = (const personality_t& other) {
    log("op=", other.moniker);
    k.reset(other.k.priv);
    moniker = other.moniker;
    if (k.pub.valid) {
        id = k.pub.hash();
    }
    else {
        id = hash_t(0);
    }
    return *this;
}

void c::reset(const priv_t& sk, const moniker_t& moniker0) {
    log("reset", moniker0);
    k.reset(sk);
    if (k.pub.valid) {
        id = k.pub.hash();
    }
    else {
        id = hash_t(0);
    }
    moniker = moniker0;
}

int c::reset_if_distinct(const string& ssk, const moniker_t& moniker0) {
    log("reset_if_distinct", moniker0);
    istringstream is(ssk);
    keys::priv_t sk;
    is >> sk;
    if (is.fail()) {
        auto r = "KO 70191 Invalid secret key.";
        log(r);
        return set_anonymous_only_key();
    }
    return reset_if_distinct(sk, moniker0);
}

int c::reset_if_distinct(const priv_t& sk, const moniker_t& moniker0) {
    log("reset_if_distinct2", moniker0);
    moniker = moniker0;
    auto r = k.reset_if_distinct(sk);
    if (r == 0) {
        return 0;
    }
    if (k.pub.valid) {
        id = k.pub.hash(); //id changed;
    }
    else {
        id = hash_t(0);
    }
    return r;
}

void c::one_liner(ostream& os) const {
    os << id << ' ' << moniker;
}

string c::one_liner() const {
    ostringstream os;
    one_liner(os);
    return os.str();
}

int c::set_anonymous_only_key() {
    if (k.priv.is_zero()) return 0; //no change
    k.reset();
    assert(!k.pub.valid);
    id.zero();
    return 1; //changed
}

int c::set_anonymous() { //moniker is irrelevant for return codes.
    moniker = "anonymous";
    return set_anonymous_only_key();
}

proof_t c::gen_proof(const challenge_t& challenge) const {
    log("gen_proof using challenge", challenge, "@", &challenge, "moniker is", moniker);
    assert(challenge.is_not_zero());
    proof_t p;
    if (!is_anonymous()) {
        sigmsg_hash_t msg_sign = proof_t::msg(id, challenge);
        p.sign(k, msg_sign);
    }
    else {
        log("not signing any proof bcs it is anon personality");
    }
    p.moniker = moniker; //moniker is information field not included in the proof
    p.id = id;
    log("generated proof for personality id", id);
    return p;
}

c::priv_t c::k_sign_docs() const {
    if (is_anonymous()) {
        log("signing using one-use random sk to prevent tampering");
        auto keys = us::gov::crypto::ec::keys::generate();
        log("TODO store private key in anonoymous_signatures file, in case a need to demonstrate tampering");
        return keys.priv;
    }
    log("using tder sk", id);
    assert(k.pub.hash() == id);
    return k.priv;
}

c c::generate(const moniker_t& moniker) {
    c o;
    o.k.reset(keys::generate_priv());
    o.id = o.k.pub.hash();
    o.moniker = moniker;
    return o;
}

c::challenge_t c::gen_challenge() {
    challenge_t o;
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&o[0]), sizeof(o));
    log("generated challenge", o);
    return o;
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(moniker) +
        blob_writer_t::blob_size(id) +
        blob_writer_t::blob_size(k);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(moniker);
    writer.write(id);
    writer.write(k);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(moniker);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(id);
        if (is_ko(r)) {
            return r;
        }
    }    
    {
        auto r = reader.read(k);
        if (is_ko(r)) {
            return r;
        }
    }    
    return ok;
}

