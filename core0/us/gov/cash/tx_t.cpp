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
#include "tx_t.h"
#include "app.h"
#include "sigcode_t.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <us/gov/config.h>

#define loglevel "gov/cash"
#define logclass "tx_t"
#include <us/gov/logs.inc>

using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::tx_t;

c::tx_t(): b(cash::app::id(), eid) {
}

c::tx_t(const tx_t& other): b(other), sections(other.sections) {
}

c::section_t& c::add_section(const hash_t& token) {
    auto i = sections.find(token);
    if (i != sections.end()) return *i;
    sections.emplace_back(section_t(token));
    return *sections.rbegin();
}

sigcode_t c::make_sigcode_all() const {
    sigcode_t i;
    i.resize(sections.size());
    int sec = 0;
    for (auto& s: sections) {
        for (int j = 0; j < s.inputs.size(); ++j) {
            i[sec].inputs.emplace_back(j);
        }
        for (int j = 0; j < s.outputs.size(); ++j) {
            i[sec].outputs.emplace_back(j);
        }
        ++sec;
    }
    i.ts = true;
    return move(i);
}

void c::write_sigmsg(sigmsg_hasher_t& h, const sigcode_t& sc) const {
    sections.write_sigmsg(h, sc);
    b::write_sigmsg(h, sc.ts);
}

string c::pay_amounts_line() const {
    ostringstream os;
    for (auto& s: sections) {
        auto tot = s.inputs.total_unsigned();
        if (tot > 0) {
            os << tot << ' ';
            if (s.token.is_zero()) os << UGAS;
            else os << s.token;
            os << ' ';
        }
    }
    return os.str();
}

void c::write_pretty_en(ostream& os) const {
    os << tx_begin_en << '\n';
    os << "type: cash\n";
    b::write_pretty_en(os);
    os << '\n';
    assert(app == cash::app::id());
    sections.write_pretty("  ", os);
    os << tx_end_en << '\n';
    os << "Transaction is ";
    ostringstream nullos;
    if (verify(nullos)) {
        os << "complete.\n";
    }
    else {
        os << "incomplete. " << nullos.str() << '\n';
    }
}

void c::write_pretty_es(ostream& os) const {
    os << tx_begin_es << '\n';
    os << "tipo: cash\n";
    b::write_pretty_es(os);
    os << '\n';
    assert(app == cash::app::id());
    sections.write_pretty("  ", os);
    os << tx_end_es << '\n';
    os << "La transaccion esta ";
    ostringstream nullos;
    if (verify(nullos)) {
        os << "completa.\n";
    }
    else {
        os << "incompleta. " << nullos.str() << '\n';
    }
}

void c::dump(const string& prefix, ostream& os) const {
    write_pretty_en(os);
}

sigmsg_hash_t c::get_hash(const sigcode_t& sc) const {
    sigmsg_hasher_t h;
    write_sigmsg(h, sc);
    sigmsg_hash_t v;
    h.finalize(v);
    return move(v);
}

bool c::check_amounts() const {
    return sections.check_amounts();
}

sigcode_t c::get_sigcode_all() const {
    sigcode_t sc;
    sc.reserve(sections.size());
    for (auto& i: sections) {
        sc.emplace_back(sigcode_section_t());
        auto& s = *sc.rbegin();
        s.inputs.reserve(i.inputs.size());
        for (int n = 0; n < i.inputs.size(); ++n) {
            s.inputs.push_back(n);
        }
        s.outputs.reserve(i.outputs.size());
        for (int n = 0; n < i.outputs.size(); ++n) {
            s.outputs.push_back(n);
        }
    }
    return move(sc);
}

bool c::verify(ostream& os) const {
    if (sections.empty()) {
        os << "No sections.\n";
        return false;
    }
    bool r = true;
    int is = 0;
    for (auto& s: sections) {
        int ii = 0;
        int io = 0;
        if (s.inputs.empty()) {
            os << "section " << is << ". No inputs\n";
            r = false;
        }
        for (auto& i: s.inputs) {
            os << "section " << is << " input " << ii << ". ";
            if (i.locking_program_input.sig.is_zero()) {
                os << "** Not signed.\n";
                r = false;
            }
            if (app::check_input(i.address, *this, i.locking_program_input)) {
                os << "Valid.\n";
            }
            else {
                os << "** Invalid.\n";
                r = false;
            }
            ++ii;
        }
        if (s.outputs.empty()) {
            os << "section " << is << ". No outputs\n";
            r = false;
        }
        ++is;
    }
    if (!r) return false;
    return b::verify(os);
}

size_t c::blob_size() const {
    return b::blob_size() + blob_writer_t::blob_size(sections);
 }

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(sections);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(sections);
        if (is_ko(r)) return r;
    }
    return ok;
}

