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
#include <b64/decode.h>

#include "m_t.h"
#include "f_t.h"
#include "t_t.h"
#include "types.h"
#include "safe_deposit_box.h"

#define loglevel "gov/cash"
#define logclass "safe_deposit_box"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::safe_deposit_box;

c::safe_deposit_box(const cash_t& v): value(v) {
}

c::safe_deposit_box(const safe_deposit_box& other): value(other.value) {
    if (unlikely(other.m != nullptr)) {
        m = new m_t(*other.m);
    }
    else {
        m = nullptr;
    }
    if (unlikely(other.f != nullptr)) {
        f = new f_t(*other.f);
    }
    else {
        f = nullptr;
    }
    if (unlikely(other.t != nullptr)) {
        t = new t_t(*other.t);
    }
    else {
        t = nullptr;
    }
}

c& c::operator = (const c& other) {
    value = other.value;
    delete m;
    if (likely(other.m == nullptr)) {
        m = nullptr;
    }
    else {
        m = new m_t(*other.m);
    }

    delete f;
    if (likely(other.f == nullptr)) {
        f = nullptr;
    }
    else {
        f = new f_t(*other.f);
    }

    delete t;
    if (likely(other.t == nullptr)) {
        t = nullptr;
    }
    else {
        t = new t_t(*other.t);
    }
    return *this;
}

c::~safe_deposit_box() {
    delete m;
    delete f;
    delete t;
}

void c::on_destroy(accounts_t& ledger) {
    if (t == nullptr) return;
    t->on_destroy(ledger);
}

void c::merge(const safe_deposit_box& other) {
    value += other.value;
    if (other.m != nullptr) {
        if (m != nullptr) {
            m->merge(*other.m);
        }
        else {
            m = new m_t(*other.m);
        }
    }
    if (other.f != nullptr) {
        if (f != nullptr) {
            f->merge(*other.f);
        }
        else {
            f = new f_t(*other.f);
        }
    }
    if (other.t != nullptr) {
        if (t != nullptr) {
            t->merge(*other.t);
        }
        else {
            t = new t_t(*other.t);
        }
    }
}

void c::dump(ostream& os) const {
    os << UGAS << ' ' << value << '\n';
    if (t == nullptr) return;
    for (auto& i: *t) {
        os << i.first << ' ' << i.second << '\n';
    }
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(value) + 1;
    if (m != nullptr) sz += blob_writer_t::blob_size(*m);
    if (f != nullptr) sz += blob_writer_t::blob_size(*f);
    if (t != nullptr) sz += blob_writer_t::blob_size(*t);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(value);
    uint8_t i = 0;
    if (m != nullptr) i |= 1;
    if (f != nullptr) i |= (1 << 1);
    if (t != nullptr) i |= (1 << 2);
    writer.write(i);
    if (m != nullptr) writer.write(*m);
    if (f != nullptr) writer.write(*f);
    if (t != nullptr) writer.write(*t);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(value);
        if (is_ko(r)) return r;
    }
    uint8_t i;
    {
        auto r = reader.read(i);
        if (is_ko(r)) return r;
    }
    {
        if ((i & 1) == 0) {
            if (m != nullptr) {
                delete m;
                m = nullptr;
            }
        }
        else {
            if (m == nullptr) m = new m_t();
            auto r = reader.read(*m);
            if (is_ko(r)) return r;
        }
    }
    {
        if ((i & (1 << 1)) == 0) {
            if (f != nullptr) {
                delete f;
                f = nullptr;
            }
        }
        else {
            if (f == nullptr) f = new f_t();
            auto r = reader.read(*f);
            if (is_ko(r)) return r;
        }
    }
    ///TODO: delete this corrective step alpha-29.20
    //if (f != nullptr) {
    //    f->del_dup();
    //}

    {
        if ((i & (1 << 2)) == 0) {
            if (t != nullptr) {
                delete t;
                t = nullptr;
            }
        }
        else {
            if (t == nullptr) t = new t_t();
            auto r = reader.read(*t);
            if (is_ko(r)) return r;
        }
    }
    return ok;
}

void c::set_supply(const hash_t& token, const cash_t& supply) {
    if (t == nullptr) {
        if (supply <= 0) return;
        t = new t_t();
    }
    t->set_supply(token, supply);
    if (unlikely(t->empty())) {
        delete t;
        t = nullptr;
    }
}

void c::print_kv_b64(ostream& os) const {
    if (unlikely(m != 0)) {
        char b[256];
        for (auto& i: *m) {
            base64::decoder d;
            int sz = d.decode(i.second.c_str(), i.second.size(), b);
            if(sz >= 0) {
                b[sz] = '\0';
                os << ' ' << i.first << ' ' << b << '\n';
            }
        }
    }
}

hash_t c::file_hash(const string& component) const {
    if (likely(f == nullptr)) return hash_t(0);
    return f->file_hash(component);
}

bool c::store(const string& key, const string& value) {
    log("MAP::STORE", key, value);
    if (m == nullptr) {
        if (unlikely(value.empty())) {
            log("fail","empty value");
            return false;
        }
        m = new m_t();
        m->emplace(key, value);
        log("emplace");
        return true;
    }
    auto i = m->find(key);
    if (i == m->end()) {
        if (!value.empty()) {
            m->emplace(key, value);
            log("emplace");
        }
        return true;
    }
    if (unlikely(value.empty())) {
        m->erase(i);
        log("erase");
        if (m->empty()) {
            delete m;
            m = nullptr;
            log("delete map");
        }
    }
    else {
        log("replace", i->second, value);
        i->second = value;
    }
    return true;
}

bool c::store(const string& path, const hash_t& filehash, uint32_t sz) {
    log("store", path, filehash, sz);
    if (f == nullptr) {
        log("f is null");
        if (unlikely(sz == 0)) return false;
        f = new f_t();
        f->emplace(filehash, fileattr_t(path, sz));
        log("f created");
        return true;
    }
    f->delete_path(path);
    auto i = f->find(filehash);
    log("found filehash?", filehash, i != f->end());
    if (i == f->end()) {
        if (likely(sz > 0)) {
            f->emplace(filehash, fileattr_t(path, sz));
            log("Added entry", filehash, path, sz);
        }
    }
    if (f->empty()) {
        log("deleted file container");
        delete f;
        f = nullptr;
    }
    return true;
/*
    if (unlikely(sz == 0)) {
        f->erase(i);
        if (f->empty()) {
            delete f;
            f = nullptr;
        }
    }
    else {
        i->second = fileattr_t(path, sz);
    }
    return true;
*/
}

bool c::burn(const hash_t& token, const cash_t& amount, cash_t& fee) {
        static const cash_t burn_fee = 1;
        fee += burn_fee;
        if (token.is_zero()) { ///GAS
            auto total_burn = amount + burn_fee;
            if (value >= total_burn) {
                value -= total_burn;
                return true;
            }
        }
        else {
            if (t == nullptr) return false;
            if (value < burn_fee) {
                return false;
            }
            value -= burn_fee; //fees in GAS
            return t->burn(token, amount);
        }
        return false;
}

void c::add(const hash_t& token, const cash_t& amount) {
    if (token.is_zero()) {
        value += amount;
    }
    else { //GAS
        if (t == nullptr) {
            t = new t_t();
            t->emplace(token, amount);
        }
        else {
            auto i = t->find(token);
            if (i == t->end()) {
                t->emplace(token, amount);
            }
            else {
                i->second += amount;
            }
        }
    }
}

void c::get_files(vector<pair<hash_t, fileattr_t>>& v) const {
    if (f == nullptr) return;
    f->get_files(v);
}

void c::get_files(vector<pair<hash_t, uint32_t>>& v) const {
    if (f == nullptr) return;
    f->get_files(v);
}

void c::get_files(const hash_t& a, vector<pair<hash_t, pair<hash_t, fileattr_t>>>& v) const {
    if (f == nullptr) return;
    f->get_files(a, v);
}

void c::get_files(set<hash_t>& v) const {
    if (f == nullptr) return;
    f->get_files(v);
}

void c::diff_files(const safe_deposit_box& newer, vector<pair<hash_t, uint32_t>>& old_files, vector<pair<hash_t, uint32_t>>& new_files) const {
    if (f == nullptr && newer.f == nullptr) return;
    if (f == nullptr && newer.f != nullptr) {
        newer.f->get_files(new_files);
        return;
    }
    if (f != nullptr && newer.f == nullptr) {
        f->get_files(old_files);
        return;
    }
    f->diff(*newer.f, old_files, new_files);
}

cash_t c::maintenance_fee() const {
    static constexpr cash_t ledger_entry_fee{1};
    static constexpr cash_t file_cost_per_mib{1};
    cash_t fee = ledger_entry_fee;
    if (unlikely(m != nullptr)) fee += m->size() * ledger_entry_fee;
    if (unlikely(f != nullptr)) fee += f->size() * ledger_entry_fee + f->total_mib();
    if (unlikely(t != nullptr)) fee += t->size() * ledger_entry_fee;
    return fee;
}

cash_t c::charge(const cash_t& tax) {
    cash_t fee = maintenance_fee();
    fee += tax;
    if (value >= fee) {
        value -= fee;
    }
    else {
        fee = value;
        value = 0;
    }
    return fee;
}

void c::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    h.write(value);
    if (m) m->hash_data_to_sign(h);
    if (f) f->hash_data_to_sign(h);
    if (t) t->hash_data_to_sign(h);
}

void c::hash_data(crypto::ripemd160& h) const {
    h.write(value);
    if (m) m->hash_data(h);
    if (f) f->hash_data(h);
    if (t) t->hash_data(h);
}

