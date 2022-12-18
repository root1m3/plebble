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
#include <sys/time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

#include "app.h"
#include "types.h"
#include "install_script_response.h"

#define loglevel "gov/sys"
#define logclass "install_script_response"
#include <us/gov/logs.inc>

using namespace us::gov::sys;
using c = us::gov::sys::install_script_response;

c::install_script_response(): b(us::gov::sys::app::id(), eid) {
    log("creating instance");
}

c::~install_script_response() {
    log("destroying instance");
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    b::hash_data_to_sign(h);
    h.write(response);
    h.write(ref);
}

void c::hash_data(hasher_t& h) const {
    b::hash_data(h);
    h.write(response);
    h.write(ref);
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: sys \n";
    b::write_pretty_en(os);
    os << '\n';
    os << "Ref: " << ref << '\n';
    os << "Response:\n" << response << '\n';
    os << tx_end_en << '\n';
    s::write_pretty_en(os);
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_es << '\n';
    os << "tipo: sys \n";
    b::write_pretty_es(os);
    os << '\n';
    os << "Ref: " << ref << '\n';
    os << "Respuesta:\n" << response << '\n';
    os << tx_end_es << '\n';
    s::write_pretty_es(os);
}

bool c::verify(ostream& os) const {
    bool ans;
    ans = b::verify(os);
    ans &= s::verify(os);
    return ans;
}

size_t c::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(response) + blob_writer_t::blob_size(ref) + s::blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(response);
    writer.write(ref);
    s::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(response);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(ref);
        if (is_ko(r)) return r;
    }
    {
        auto r = s::from_blob(reader);
        if (is_ko(r)) return r;
    }
    return ok;
}

