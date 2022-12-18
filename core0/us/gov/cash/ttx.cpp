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
#include "app.h"
#include "ttx.h"

#define loglevel "gov/cash"
#define logclass "ttx"
#include <us/gov/logs.inc>

using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::ttx;
using us::ko;

c::ttx(): b(cash::app::id(), eid) {
}

void c::write_sigmsg(sigmsg_hasher_t& h) const {
    b::write_sigmsg(h, true);
    h.write(address);
    h.write(token);
    h.write(supply);
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: cash\n";
    b::write_pretty_en(os);
    os << '\n';
    os << "address: " << address << '\n';
    os << "unlock with: " << locking_program_input.encode() << '\n';
    os << "coin: " << token << '\n';
    os << "supply: " << supply << '\n';
    os << tx_end_en << '\n';
    os << "Transaction is ";
    ostringstream osv;
    if (verify(osv)) {
        os << "complete.\n";
    }
    else {
        os << "incomplete. " << osv.str() << '\n';
    }
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_es << '\n';
    os << "tipo: cash\n";
    b::write_pretty_es(os);
    os << '\n';
    os << "direccion: " << address << '\n';
    os << "abrir con: " << locking_program_input.encode() << '\n';
    os << "moneda: " << token << '\n';
    os << "cantidad: " << supply << '\n';
    os << tx_end_es << '\n';
    os << "La transaccion esta ";
    ostringstream osv;
    if (verify(osv)) {
        os << "completa.\n";
    }
    else {
        os << "incompleta. " << osv.str() << '\n';
    }
}

sigmsg_hash_t c::get_hash() const {
    sigmsg_hasher_t h;
    write_sigmsg(h);
    sigmsg_hash_t v;
    h.finalize(v);
    return move(v);
}

bool c::verify(ostream& os) const {
    return b::verify(os);
}

size_t c::blob_size() const {
    return b::blob_size() +
    blob_writer_t::blob_size(address) +
    blob_writer_t::blob_size(locking_program_input) +
    blob_writer_t::blob_size(token) +
    blob_writer_t::blob_size(supply);
 }

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(address);
    writer.write(locking_program_input);
    writer.write(token);
    writer.write(supply);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(address);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(locking_program_input);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(token);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(supply);
        if (is_ko(r)) return r;
    }
    return ok;
}

pair<ko, c*> c::from_b58(const string& s) {
    blob_t blob = crypto::b58::decode(s);
    c* o = new c();
    auto r = o->read(blob);
    if (is_ko(r)) {
        delete o;
        auto r = "KO 54093 Invalid tx";
        return make_pair(r, nullptr);
    }
    return make_pair(ok, o);
}

string c::to_b58() const {
    blob_t blob;
    write(blob);
    return crypto::b58::encode(blob);
}

