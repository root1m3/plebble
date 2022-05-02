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
#include "proof_t.h"
#include <us/gov/crypto/base58.h>
#include <cassert>
#include <sstream>

#define loglevel "wallet/trader/personality"
#define logclass "proof"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet::trader::personality;
using c = us::wallet::trader::personality::proof_t;
using us::ko;

c::sigmsg_hash_t c::msg(const hash_t& id, const challenge_t& peer_challenge) {
    sigmsg_hasher_t hasher;
    hasher.write(id);
    hasher.write(peer_challenge);
    sigmsg_hasher_t::value_type msg_sign;
    hasher.finalize(msg_sign);
    return msg_sign;
}

bool c::verify(const challenge_t& challenge, ostream& os) const {
    log("verify proof using challenge", challenge, "@", &challenge);
    if (id.is_zero()) {
        log("id is anonymous, it doesnt require signatures, verified.");
        return true;
    }
    auto m = msg(id, challenge);
    return b::verify(m, os);
}

bool c::verify(const challenge_t& peer_challenge) const {
    ostream os(0);
    return verify(peer_challenge, os);
}

size_t c::blob_size() const {
    return b::blob_size() + b2::blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    b2::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = b2::from_blob(reader);
        if (is_ko(r)) return r;
    }
    return ok;
}

c::raw_t c::raw() const {
    raw_t r;
    r = *this;
    return move(r);
}

/// -------------------------------------------------------------------------------------------------------

using c1 = us::wallet::trader::personality::proof_t::raw_t;

c1::raw_personality_t(): id(0), moniker("anonymous") {
}

c1::raw_personality_t(const raw_personality_t& other): id(other.id), moniker(other.moniker) {
}

c1::raw_personality_t(const hash_t& id_, const moniker_t& moniker_) {
    id = id_;
    moniker = moniker_;
}

c1::raw_personality_t(hash_t&& id_, moniker_t&& moniker_) {
    id = move(id_);
    moniker = move(moniker_);
}

bool c1::reset(raw_personality_t&& other) {
    hash_t prev = id;
    *this = move(other);
    return prev != id;
}

void c1::dump(ostream& os) const {
    os << "id: " << id << '\n';
    os << "moniker: " << moniker << '\n';
}

void c1::dump(const string& pfx, ostream& os) const {
    os << pfx << "id:" << id << " moniker:" << moniker << '\n';
}

string c1::to_string() const {
    ostringstream os;
    dump("", os);
    return os.str();
}

size_t c1::blob_size() const {
    return blob_writer_t::blob_size(id) +
        blob_writer_t::blob_size(moniker);
}

void c1::to_blob(blob_writer_t& writer) const {
    writer.write(id);
    writer.write(moniker);
}

ko c1::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(id);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(moniker);
        if (is_ko(r)) return r;
    }
    return ok;
}

