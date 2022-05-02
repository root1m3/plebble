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
#include "local_delta.h"
#include "app.h"
#include "account_t.h"

#define loglevel "gov/cash"
#define logclass "local_delta"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::local_delta;
using us::ko;

size_t std::hash<c>::operator() (const c& g) const {
    return *reinterpret_cast<const size_t*>(&g.get_hash()[0]);
}

c::local_delta() {
}

c::local_delta(const local_delta& other): accounts(other.accounts), fees(other.fees), hash(other.hash) {
}

c::~local_delta() {
}

c& c::operator = (int zero) {
    *this = local_delta();
    return *this;
}

appid_t c::app_id() const {
    return app::id();
}

void c::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    for (auto& i: accounts) {
        h.write(i.first);
        i.second.hash_data_to_sign(h);
    }
    h.write(fees);
}

void c::hash_data(crypto::ripemd160& h) const {
    for (auto& i: accounts) {
        h.write(i.first);
        i.second.hash_data(h);
    }
    h.write(fees);
}

void c::dump(const string& prefix, ostream& os) const {
    accounts.dump(prefix, 2, os);
    os << prefix << "fees: " << fees << '\n';
}

const hash_t& c::get_hash() const {
    if (hash.is_zero()) hash = compute_hash();
    return hash;
}

bool c::operator == (const c& other) const {
    if (*this != other) return false;
    return true;
}

bool c::operator != (const c& other) const {
    if (fees != other.fees) return false;
    if (accounts.size() != other.accounts.size()) return false;
    for (auto l = accounts.begin(), r = other.accounts.begin(); l != accounts.end(); ++l, ++r) {
        if (l->first != r->first) return false;
        if (l->second.box.value != r->second.box.value) return false;
        if (l->second.locking_program != r->second.locking_program) return false;
    }
    return true;
}

hash_t c::compute_hash() const {
    hasher_t h;
    for (auto& i: accounts) {
        h.write(i.first);
        h.write(i.second.box.value);
        h.write(i.second.locking_program);
    }
    h.write(fees);
    hash_t v;
    h.finalize(v);
    return move(v);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(accounts) + blob_writer_t::blob_size(fees);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(accounts);
    writer.write(fees);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(accounts);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(fees);
        if (is_ko(r)) return r;
    }
    return ok;
}

