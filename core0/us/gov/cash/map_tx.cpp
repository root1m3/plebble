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
#include "map_tx.h"
#include "app.h"
#include "types.h"

#define loglevel "gov/cash"
#define logclass "map_tx"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::map_tx;

c::map_tx(): b(app::id(), eid) {
}

void c::write_sigmsg(sigmsg_hasher_t& h) const {
    b::write_sigmsg(h, true);
    h.write(address);
    h.write(key);
    h.write(value);
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: cash\n";
    b::write_pretty_en(os);
    os << '\n';
    os << "address: " << address << '\n';
    os << "unlock with: " << locking_program_input.encode() << '\n';
    os << "key: " << key << '\n';
    os << "value: " << value << '\n';
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
    os << "llave: " << key << '\n';
    os << "valor: " << value << '\n';
    os << tx_end_en << '\n';
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

size_t c::blob_size() const {
    return b::blob_size() +
    blob_writer_t::blob_size(address) +
    blob_writer_t::blob_size(locking_program_input) +
    blob_writer_t::blob_size(key) +
    blob_writer_t::blob_size(value);
 }

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(address);
    writer.write(locking_program_input);
    writer.write(key);
    writer.write(value);
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
        auto r = reader.read(key);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(value);
        if (is_ko(r)) return r;
    }
    return ok;
}

