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
#include "db_t.h"
#include "accounts_t.h"
#include "types.h"

#include <us/gov/config.h>

#define loglevel "gov/cash"
#define logclass "db_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::db_t;

hash_t c::file_hash(const hash_t& addr, const string& path) const {
    return accounts->file_hash(addr, path);
}

ko c::print_data(const hash_t& addr, ostream& os) const {
    return accounts->print_data(addr, os);
}

void c::list_files(const hash_t& addr, const string& path, ostream& os) const {
    accounts->list_files(addr, path, os);
}

void c::dump(const string& prefix, int detail, ostream& os) const {
    lock_guard<mutex> lock(mx);
    accounts->dump(prefix, detail, os);
    if (detail > 1) {
        auto cir = cash_in_circulation();
        os << prefix << "circulation " << cir << '\n';
        os << prefix << "max_print per cycle " << max_subsidy << '\n';
    }
}

bool c::add_(const hash_t& k, const cash_t& amount) {
    return accounts->pay(k, amount);
}

bool c::withdraw_(const hash_t& k, const cash_t& amount) {
    return accounts->withdraw(k, amount);
}

cash_t c::cash_in_circulation() const {
    return accounts->sum();
}

c::db_t() {
    clear();
}

void c::clear() {
    lock_guard<mutex> lock(mx);
    delete accounts;
    accounts = new accounts_t();
    max_subsidy = CFG_MAX_SUBSIDY; //change implies setting tip to snapshot (not a delta) on next upgrade
}

c::db_t(db_t&& other):max_subsidy(other.max_subsidy) {
    delete accounts;
    accounts = other.accounts;
    other.accounts = nullptr;
}

c::~db_t() {
    delete accounts;
}

void c::print_kv_b64(const hash_t& addr, ostream& os) const {
    accounts->print_kv_b64(addr, os);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(*accounts) + blob_writer_t::blob_size(max_subsidy);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(*accounts);
    writer.write(max_subsidy);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(*accounts);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(max_subsidy);
        if (is_ko(r)) return r;
    }
    return ok;
}

