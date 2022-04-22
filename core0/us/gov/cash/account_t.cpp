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
#include "account_t.h"

#include <us/gov/types.h>
#include <us/gov/engine/daemon_t.h>

#include "app.h"
#include "t_t.h"
#include "f_t.h"
#include "m_t.h"
#include "types.h"

#define loglevel "gov/cash"
#define logclass "account_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::account_t;

c::account_t(): locking_program(app::locking_program_id), box(0) {
}

c::account_t(const locking_program_t& locking_program, const box_t& box): locking_program(locking_program), box(box) {
}

c::account_t(locking_program_t&& locking_program, box_t&& box): locking_program(move(locking_program)), box(move(box)) {
}

void c::on_destroy(accounts_t& ledger) {
    box.on_destroy(ledger);
}

void c::merge(const account_t& other) {
    box.merge(other.box);
}

cash_t c::get_value(const hash_t& token) const {
    if (token.is_zero()) return box.value;
    if (box.t == nullptr) return 0;
    return box.t->get_value(token);
}

cash_t c::input_eligible(const hash_t& token) const { //returns available token
    log("input_eligible");
    auto fee = box.maintenance_fee();
    if (box.value <= fee) {
        log("negligible sum", box.value, fee);
        return 0;
    }
    if (token.is_zero()) {
    auto n = box.value - fee;
        if (box.f == nullptr && box.t == nullptr && box.m == nullptr) {
        log("light. gas", n);
            return n;
        }
    if (n < 1000000) { //TODO this limit to static conf
        log("under 1000000 gas. not eligible.");
            return 0; //accounts holding data are not taken away gas if they 're under 1000000
    }
    log("eligible gas", n - 1000000);
    return n - 1000000;
    }
    if (box.t == nullptr) return 0;
    auto i = box.t->find(token);
    if (i == box.t->end()) {
        return 0;
    }
    return i->second;
}

cash_t c::input_eligible2(const hash_t& token) const { //returns available token
    log("input_eligible2");
    auto fee = box.maintenance_fee();
    if (box.value <= fee) {
        log("negligible dust", box.value, fee);
        return 0;
    }
    if (token.is_zero()) {
    auto n = box.value - fee;
    if (n < 1000000) { //TODO this limit to static conf
        log("under 1000000 gas. not eligible.");
            return 0; //accounts holding data are not taken away gas if they 're under 1000000
    }
    log("eligible gas", n - 1000000);
    return n - 1000000;
    }
    if (box.t == nullptr) return 0;
    auto i = box.t->find(token);
    if (i == box.t->end()) {
    log("no token");
        return 0;
    }
    return i->second;
}

void c::dump(const string& prefix, const hash_t& addr, int detail, ostream& os) const {
    os << prefix << addr << ' ';
    if (detail > 1) {
        if (unlikely(locking_program != app::locking_program_id)) {
            os << "(locking_program " << +locking_program << ") ";
        }
    }
    os << UGAS << ' ' << box.value;
    if (detail == 0) {
        if (box.m != nullptr || box.f != nullptr || box.t != nullptr) {
            os << " *\n";
        }
        else {
            os << '\n';
        }
        return;
    }
    if (unlikely(box.m != nullptr)) {
        os << "; " << box.m->size() << " kv records";
    }
    if (unlikely(box.f != nullptr)) {
        os << "; " << box.f->size() << " files. " << box.f->total_mib() << " MiB";
    }
    if (unlikely(box.t != nullptr)) {
        os << "; " << box.t->size() << " coins";
    }
    os << '\n';
    if (detail > 1) {
        string pfx =  prefix + "    ";
        string pfx2 =  pfx + "    ";
        if (box.t != nullptr) {
            os << pfx << "Coins:\n";
            box.t->dump(pfx2, addr, os);
        }
        if (box.m != nullptr) {
            os << pfx << "Key-value:\n";
            box.m->dump(pfx2, os);
        }
        if (box.f != nullptr) {
            os << pfx + "Files:\n";
            box.f->dump(pfx2, os);
        }
    }
}

void c::list_files(const string& path, ostream& os) const {
    if (box.f == nullptr) return;
    box.f->list_files(path, os);
}

hash_t c::file_hash(const string& path) const {
    if (box.f == nullptr) return hash_t(0);
    return box.f->file_hash(path);
}

void c::print_data(const hash_t& addr, ostream& os) const {
    os << "Account " << addr << '\n';
    os << "locking_program " << +locking_program << '\n';
    os << UGAS << ' ' << box.value << '\n';
    if (box.m != nullptr) {
        os << box.m->size() << " key-value records:\n";
        box.m->dump("  ", os);
    }
    if (box.f != nullptr) {
        os << box.f->size() << " files. Total size " << box.f->total_mib() << " MiB.\n";
        box.f->dump("  ", os);
    }
    if (unlikely(box.t != nullptr)) {
        os << box.t->size() << " coins:\n";
        box.t->dump("  ", addr, os);
    }
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    h.write(locking_program);
    box.hash_data_to_sign(h);
}

void c::hash_data(hasher_t& h) const {
    h.write(locking_program);
    box.hash_data(h);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(locking_program) + blob_writer_t::blob_size(box);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(locking_program);
    writer.write(box);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(locking_program);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(box);
        if (is_ko(r)) return r;
    }
    return ok;
}

