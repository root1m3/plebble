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
#include "business.h"
#include "trader_protocol.h"

#define loglevel "wallet/trader"
#define logclass "business"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::business_t;

const char* c::KO_50100 = "KO_50100 Invalid function.";

c::business_t() {
    log("created business_t at", this);
}

c::~business_t() {
    log("destroyed business_t at", this);
}

ko c::init(const string& r2rhome_, protocol_factories_t& f) {
    r2rhome = r2rhome_;
    if (r2rhome.empty()) {
        auto r = "KO 20102 Homebase variable is not set.";
        log(r);
        return r;
    }
    home = homedir();
    us::gov::io::cfg0::ensure_dir(home);
    log("loading bz name from", home + "/name");
    { //load business name
        ifstream is(home + "/name");
        getline(is, name);
        us::gov::io::cfg0::trim(name);
        if (is.fail()) {
            is.close();
            ofstream ofs(home + "/name");
            ofs << name << '\n';
            log("saved default business_name", name, home + "/name");
        }
    }
    log("loading bz ico from", home + "/ico.png");
    { //load ico
        auto r = us::gov::io::read_file_(home + "/ico.png", ico);
        if (is_ko(r)) {
            log("Unable to read business ico from file", home + "/ico.png");
            assert(ico.empty());
        }
    }
    log("ico sz", ico.size(), "name", name);
    register_factories(f);
    return ok;
}

void c::exec_help(const string& prefix, ostream& os) const {
    trader_protocol::exec_help(prefix, os);
}

ko c::exec(istream& is, wallet::local_api& w) {
    return trader_protocol::exec(is, w);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << name << '\n';
}

pair<c::protocol_selection_t, c::bookmark_info_t> c::bookmark_info() const {
    pair<protocol_selection_t, bookmark_info_t> r;
    protocols_t p;
    published_protocols(p, false);
    assert(!p.empty());
    assert(p.size() == 1);
    r.first = *p.begin();
    r.second.label = name;
    r.second.ico = ico;
    return move(r);
}

c::protocol_selection_t c::protocol_selection() const {
    protocols_t p;
    published_protocols(p, false);
    assert(!p.empty());
    return *p.begin();
}

