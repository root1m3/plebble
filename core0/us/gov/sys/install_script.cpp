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
#include "install_script.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

#include "app.h"
#include "types.h"

#define loglevel "gov/sys"
#define logclass "install_script"
#include <us/gov/logs.inc>

using namespace us::gov::sys;
using c = us::gov::sys::install_script;

c::install_script(): b(us::gov::sys::app::id(), 0) {
    log( "creating install_script" );
}

c::~install_script() {
    log("destroying install_script");
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    b::hash_data_to_sign(h);
    h.write(script);
}

void c::hash_data(ripemd160& h) const {
    b::hash_data(h);
    h.write(script);
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: sys\n";
    b::write_pretty_en(os);
    os << '\n';
    os << "Script:\n";
    os << script << '\n';
    os << tx_end_en << '\n';
    s::write_pretty_en(os);
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_es << '\n';
    os << "type: sys\n";
    b::write_pretty_es(os);
    os << '\n';
    os << "Script:\n";
    os << script << '\n';
    os << tx_end_en << '\n';
    s::write_pretty_es(os);
}

bool c::verify(ostream& os) const {
    bool ans;
    ans = b::verify(os);
    ans &= s::verify(os);
    return ans;
}

size_t c::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(script) + s::blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(script);
    s::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(script);
        if (is_ko(r)) return r;
    }
    {
        auto r = s::from_blob(reader);
        if (is_ko(r)) return r;
    }
    return ok;
}

