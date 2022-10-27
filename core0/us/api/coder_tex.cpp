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
#include "coder_tex.h"

#include <sstream>
#include <cassert>
#include <fstream>

#include "api_t.h"
#include "apifun.h"

using namespace std;
using namespace us::apitool;
using c = us::apitool::tex::coder;

void c::do_fn(const apifun&, ostream& os) const {
    os << "  " << "blab bla" << endl;
}


void c::generate(const api_t& a) const {
    ostringstream fn;
    write_file_prefix(fn);
    fn << a.name;
    string file = fn.str();
    feedback();
    ofstream os(file);
    api_t::warn_h(line_comment(), os);
    os << "Src file " << a.src << endl;
    for (auto& i: a) {
        os << "Function Name " << i.name << endl;
        do_fn(i, os);
    }
    api_t::warn_f(line_comment(), os);

}

void c::generate() const {
    for (auto& i: m) {
        generate(*i);
    }
/*
    ostringstream fn;
    write_file_prefix(fn);
    fn << "datagram_counters";
    string file = fn.str();
    feedback(fn.str());
    ofstream include_os(file);
    api_t::warn_h(include_os);
    for (auto& i: m) {
        do_gov_daemon_api(*i.first, i.second, include_os);
    }
    api_t::warn_f(include_os);
*/
}

void c::write_svcfish_entry(const svcfish_entry_t& x, ostream& os) const {
    if (x.op == '/') {
        os << line_comment() << ' ';
        write_svcfish_entry_comment(x, os);
        return;
    }
    os << "{" << x.from_svc << ", " << x.to_svc << "}, ";
    os << line_comment() << ' ';
    write_svcfish_entry_comment(x, os);
    return;
}


