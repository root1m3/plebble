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
#include "c_t.h"
#include <us/gov/crypto/base58.h>
#include <cassert>
#include <sstream>
#include <functional>
#include "../personality/personality_t.h"

#define loglevel "wallet/trader/bootstrap"
#define logclass "c_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us;
using namespace us::wallet::trader::bootstrap;
using c1_t = us::wallet::trader::bootstrap::c1_t;
using c2_t = us::wallet::trader::bootstrap::c2_t;
using c3_t = us::wallet::trader::bootstrap::c3_t;

c1_t::c1_t() {
}

c1_t::c1_t(const endpoint_t& endpoint, const string& wloc, protocol_selection_t&& protocol_selection, params_t&& params, const challenge_t& challenge):
    endpoint(move(endpoint)), wloc(wloc), protocol_selection(move(protocol_selection)), params(params), challenge(challenge) {
}

size_t c1_t::blob_size() const {
    return blob_writer_t::blob_size(endpoint) +
    blob_writer_t::blob_size(wloc) +
    blob_writer_t::blob_size(protocol_selection) +
    blob_writer_t::blob_size(params) +
    blob_writer_t::blob_size(challenge);
}

void c1_t::to_blob(blob_writer_t& writer) const {
    writer.write(endpoint);
    writer.write(wloc);
    writer.write(protocol_selection);
    writer.write(params);
    writer.write(challenge);
}

ko c1_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(endpoint);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(wloc);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(protocol_selection);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(params);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(challenge);
        if (is_ko(r)) return r;
    }
    return ok;
}

//====================================================

c2_t::c2_t() {
}

c2_t::c2_t(endpoint_t&& endpoint, params_t&& params, personality_proof_t&& personality_proof, const challenge_t& challenge): b(move(endpoint), "", move(protocol_selection_t()), move(params), challenge), personality_proof(move(personality_proof)) {
}

c2_t::c2_t(const endpoint_t& endpoint, params_t&& params, personality_proof_t&& personality_proof, const challenge_t& challenge): b(endpoint, "", move(protocol_selection_t()), move(params), challenge), personality_proof(move(personality_proof)) {
}

size_t c2_t::blob_size() const {
    return b::blob_size() +
        blob_writer_t::blob_size(personality_proof);
}

void c2_t::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(personality_proof);
}

ko c2_t::from_blob(blob_reader_t& reader) {
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

