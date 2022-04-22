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
#include "a_t.h"
#include <us/gov/crypto/base58.h>
#include <cassert>
#include <sstream>
#include <functional>
#include "../personality/personality_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "a_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::wallet::trader::bootstrap;
using a1_t = us::wallet::trader::bootstrap::a1_t;
using a2_t = us::wallet::trader::bootstrap::a2_t;
using a3_t = us::wallet::trader::bootstrap::a3_t;
using us::ko;

a1_t::a1_t() {
}

a1_t::a1_t(const endpoint_t& endpoint, const string& wloc, protocols_t&& protocols, const challenge_t& challenge):
    endpoint(endpoint), wloc(wloc), protocols(move(protocols)), challenge(challenge) {
}

size_t a1_t::blob_size() const {
    return blob_writer_t::blob_size(endpoint) +
        blob_writer_t::blob_size(wloc) +
        blob_writer_t::blob_size(protocols) +
        blob_writer_t::blob_size(challenge);
}

void a1_t::to_blob(blob_writer_t& writer) const {
    writer.write(endpoint);
    writer.write(wloc);
    writer.write(protocols);
    writer.write(challenge);
}

ko a1_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(endpoint);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(wloc);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(protocols);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(challenge);
        if (is_ko(r)) return r;
    }
    return ok;
}

//=_===================================================

a2_t::a2_t() {
}

a2_t::a2_t(const endpoint_t& endpoint, protocols_t&& protocols, personality_proof_t&& personality_proof, const challenge_t& challenge): b(endpoint, "", move(protocols), challenge), personality_proof(move(personality_proof)) {
}

size_t a2_t::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(personality_proof);
}

void a2_t::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(personality_proof);
}

ko a2_t::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(personality_proof);
        if (is_ko(r)) return r;
    }
    return ok;
}


//=_===================================================

a3_t::a3_t() {
}

a3_t::a3_t(personality_proof_t&& personality_proof): personality_proof(move(personality_proof)) {
}

size_t a3_t::blob_size() const {
    return blob_writer_t::blob_size(personality_proof);
}

void a3_t::to_blob(blob_writer_t& writer) const {
    writer.write(personality_proof);
}

ko a3_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(personality_proof);
        if (is_ko(r)) return r;
    }
    return ok;
}

