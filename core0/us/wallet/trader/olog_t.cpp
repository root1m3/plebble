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
#include "olog_t.h"
#include <us/gov/io/cfg0.h>

#define loglevel "wallet/trader"
#define logclass "olog_t"
#include <us/gov/logs.inc>

using c = us::wallet::trader::olog_t;
using namespace us::wallet::trader;
using namespace std;
using namespace std::chrono;
using us::ko;

olog_t::~olog_t() {
    delete logos;
}

string olog_t::ts() {
    using namespace chrono;
    return ts(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
}

string olog_t::ts(uint64_t t) {
    ostringstream os;
    os << t;
    string s = os.str();
    if (likely(s.size()) > 1) {
        auto i = s.rbegin();
        auto j = i;
        ++j;
        for (int n = 0; n < 8; ++n) {
            *i = *j;
            ++i; ++j;
            if (j == s.rend()) break;
        }
        *i = '.';
    }
    return s;
}

void c::init_olog(const string& home, const string& filename) {
    log("init_olog", home, filename);
    ostringstream logfn;
    logfn << home << "/log";
    us::gov::io::cfg0::ensure_dir(logfn.str());
    logfn << filename;
    ologfile = logfn.str();
    log("init_olog", ologfile, logos);
    assert(logos == nullptr);
    logos = new ofstream(ologfile, ios::app);
    assert(logos->good());
    *logos << ts() << ' ' << "Create\n";
    logos->flush();
}

void c::show_log(const hash_t& id, ostream& os) const {
    log("show_log", id, ologfile);
    os << "==== log trade " << id << '\n';
    ifstream is(ologfile);
    assert(!is.fail());
    os << is.rdbuf() << '\n';
    os << "=/== end log trade " << id << '\n';
    assert(!is.fail());
}

