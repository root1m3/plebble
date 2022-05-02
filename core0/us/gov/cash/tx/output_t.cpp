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
#include "output_t.h"
#include <us/gov/crypto/base58.h>
#include <us/gov/cash/sigcode_section_t.h>

#define loglevel "gov/cash/tx"
#define logclass "output_t"
#include <us/gov/logs.inc>

using c = us::gov::cash::tx::output_t;
using us::ko;
using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

c::output_t() {
}

c::output_t(const hash_t& address, const cash_t& amount): address(address), amount(amount) {
}

c::output_t(const output_t& other): address(other.address), amount(other.amount) {
}

void c::write_sigmsg(sigmsg_hasher_t& h) const {
    h.write(address);
    h.write(amount);
}

void c::write_pretty(const string& prefix, ostream& os) const {
    os << prefix << "address: " << address << '\n';
    os << prefix << "amount: " << amount << '\n';
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(address) + blob_writer_t::blob_size(amount);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(address);
    writer.write(amount);
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
    return ok;
}

