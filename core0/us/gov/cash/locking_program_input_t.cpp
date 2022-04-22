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
#include "locking_program_input_t.h"
#include <us/gov/crypto/base58.h>
#include "types.h"

#define loglevel "gov/cash"
#define logclass "locking_program_input_t"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::locking_program_input_t;

c::locking_program_input_t() {
}

c::locking_program_input_t(const c& other): sigcode(other.sigcode), pubkey(other.pubkey), sig(other.sig) {
}

c& c::operator = (const c& other) {
    sigcode = other.sigcode;
    pubkey = other.pubkey;
    sig = other.sig;
    return *this;
}

c::~locking_program_input_t() {
}

void c::dump(ostream& os) const {
    os << "sigcode\n";
    sigcode.dump(os);
    os << "pubkey " << pubkey << '\n';
    os << "sig " << sig << '\n';
}

void c::dump_line(ostream& os) const {
    if (sig.is_zero()) {
        os << '-';
        return;
    }
    sigcode.dump_line(os);
    os << "pub " << pubkey << " sig " << sig;
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(sigcode) + blob_writer_t::blob_size(pubkey) + blob_writer_t::blob_size(sig);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(sigcode);
    writer.write(pubkey);
    writer.write(sig);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(sigcode);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(pubkey);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(sig);
        if (is_ko(r)) return r;
    }
    return ok;
}

