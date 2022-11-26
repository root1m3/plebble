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
#include <us/gov/cash/sigcode_section_t.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>

#include "input_t.h"

#define loglevel "gov/cash/tx"
#define logclass "input_t"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::tx::input_t;
using us::ko;

c::input_t() {
}

c::input_t(const hash_t& address, const cash_t& amount): address(address), amount(amount) {
}

c::input_t(const hash_t& address, const cash_t& amount, const locking_program_input_t& locking_program_input): address(address), amount(amount), locking_program_input(locking_program_input) {
}

c::input_t(const input_t& other): address(other.address), amount(other.amount), locking_program_input(other.locking_program_input) {
}


void c::write_sigmsg(sigmsg_hasher_t& h) const {
    h.write(address);
    h.write(amount);
}

void c::write_pretty(const string& prefix, ostream& os) const {
    os << prefix << "address: " << address << '\n';
    os << prefix << "withdraw: " << amount << '\n';
    os << prefix << "unlock with: ";
    locking_program_input.dump_line(os);
    os << '\n';
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(address) + blob_writer_t::blob_size(amount) + blob_writer_t::blob_size(locking_program_input);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(address);
    writer.write(amount);
    writer.write(locking_program_input);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(address);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(amount);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(locking_program_input);
        if (is_ko(r)) return r;
    }
    return ok;
}

