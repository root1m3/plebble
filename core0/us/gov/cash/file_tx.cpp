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
#include <sstream>

#include "types.h"
#include "app.h"
#include "file_tx.h"

#define loglevel "gov/cash"
#define logclass "file_tx"
#include <us/gov/logs.inc>

using namespace us::gov::cash;
using c = us::gov::cash::file_tx;

c::file_tx(): b(cash::app::id(), eid) {
}

void c::write_sigmsg(sigmsg_hasher_t& h) const {
    b::write_sigmsg(h, true);
    h.write(address);
    h.write(digest);
    h.write(size);
    h.write(path);
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: cash\n";
    b::write_pretty_en(os);
    os << '\n';
    os << "address: " << address << '\n';
    os << "unlock with: " << locking_program_input.encode() << '\n';
    os << "path: " << path << '\n';
    os << "file name: " << digest << '\n';
    os << "file size: " << size << " bytes\n";
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
    os << "camino: " << path << '\n';
    os << "nombre fichero: " << digest << '\n';
    os << "tamano fichero: " << size << " bytes\n";
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

size_t c::blob_size() const {
    return b::blob_size() +
    blob_writer_t::blob_size(address) +
    blob_writer_t::blob_size(locking_program_input) +
    blob_writer_t::blob_size(digest) +
    blob_writer_t::sizet_size(size) +
    blob_writer_t::blob_size(path);
 }

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(address);
    writer.write(locking_program_input);
    writer.write(digest);
    writer.write_sizet(size);
    writer.write(path);
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
        auto r = reader.read(digest);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read_sizet(size);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(path);
        if (is_ko(r)) return r;
    }
    return ok;
}

