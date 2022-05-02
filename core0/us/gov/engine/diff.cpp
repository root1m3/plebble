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
#include "diff.h"

#include <cassert>
#include <iomanip>
#include <vector>

#include <us/gov/likely.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/engine/db_t.h>
#include <us/gov/io/cfg0.h>

#include "app.h"
#include "local_deltas_t.h"

#define loglevel "gov/engine"
#define logclass "diff"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::diff;

c::~diff() {
    clear();
}

void c::clear() {
    for (auto& i: *this) delete i.second;
    b::clear();
}

uint64_t c::add(int appid, app::local_delta* g) {
    log("adding localdelta for app", appid);
    auto i = find(appid);
    if (unlikely(i == end())) {
        log("first time, creating new delta");
        auto a = app::delta::create(appid);
        if (unlikely(a == nullptr)) {
            log("unexpected! cannot create delta");
            return 0;
        }
        i = emplace(appid, a).first;
    }
    return i->second->merge(g);
}

bool c::add(local_deltas_t* ld) {
    assert(ld != nullptr);
    log("adding local_deltas", ld->id);
    const auto& pubkeyh = ld->pubkey.hash();
    {
        lock_guard<mutex> lock(mx_proof_of_work);
        log("looking for proof_of_work for", pubkeyh);
        auto i = proof_of_work.find(pubkeyh);
        if (i != proof_of_work.end()) {
            log("local_deltas from node", pubkeyh, "is already processed.");
            delete ld;
            return false;
        }
    }
    uint64_t work = 0;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: *ld) {
            work += add(i.first, i.second);
            i.second = nullptr;
        }
    }
    {
        lock_guard<mutex> lock(mx_proof_of_work);
        log("registered proof_of_work for ", pubkeyh, "work", work);
        proof_of_work.emplace(pubkeyh, work);
    }
    id = ld->id;
    delete ld;
    return true;
}

void c::close() {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        i.second->end_merge();
    }
    h = false;
}

const hash_t& c::hash() const { //TODO: call write(hasher) bcs it's more ifficient if instead serializing to a vector and then hash it
    if (!h) {
        blob_t blob;
        write(blob);
        hasher_t hasher;
        hasher.write(blob);
        hasher.finalize(hash_cached);
        h = true;
    }
    return hash_cached;
}

const hash_t& c::hash(const blob_t& blob) const {
    if (!h) {
        hasher_t hasher;
        hasher.write(blob);
        hasher.finalize(hash_cached);
        h = true;
    }
    return hash_cached;
}

uint64_t pow_t::sum() const {
    uint64_t s = 0;
    for (auto& i: *this) {
        s += i.second;
    }
    return s;
}

void pow_t::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << i.first << ' ' << i.second << '\n';
    }
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "previous hash " << prev << '\n';
    os << prefix << "id " << id << '\n';
    os << prefix << "Verification work:\n";
    proof_of_work.dump(prefix + "    ", os);
    os << prefix << size() << " apps:\n";
    string pfx = prefix + "    ";
    string pfx2 = pfx + "    ";
    for (auto& i: *this) {
        os << pfx << "app " << +i.first << ":\n";
        i.second->dump(pfx2, os);
    }
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(prev) + blob_writer_t::blob_size(id);
    sz += blob_writer_t::sizet_size(size());
    for (auto& i: *this) {
        sz += blob_writer_t::blob_size(i.first);
        sz += blob_writer_t::blob_size(*i.second);
    }
    sz += blob_writer_t::blob_size(proof_of_work);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(prev);
    writer.write(id);
    writer.write_sizet(size());
    for (auto& i: *this) {
        writer.write(i.first);
        writer.write(*i.second);
    }
    writer.write(proof_of_work);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(prev);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(id);
        if (is_ko(r)) return r;
    }
    {
        clear();
        uint64_t sz;
        auto r = reader.read_sizet(sz);
        if (is_ko(r)) return r;
        if (unlikely(sz > db_t::num_apps)) return "KO 78191 Too many apps.";
        for (uint64_t i = 0; i < sz; ++i) {
            appid_t a;
            {
                auto r = reader.read(a);
                if (is_ko(r)) return r;
            }
            log("read app id", a, "...");
            app::delta* b = app::delta::create(a);
            if (unlikely(b == nullptr)) {
                auto r = "KO 89700 Invalid app id in diff.";
                log(r);
                return r;
            }
            {
                auto r = reader.read(*b);
                if (is_ko(r)) return r;
            }
            log("read app data OK.", a);
            emplace(a, b);
        }
    }
    {
        auto r = reader.read(proof_of_work);
        if (is_ko(r)) return r;
    }
    return ok;
}

