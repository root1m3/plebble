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

#include <fstream>

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/trader/trader_t.h>

#define loglevel "trader/r2r/games"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::trader::r2r::games::business_t;

c::business_t() {
    log("constructor games bz");
}

c::~business_t() {
    log("destroyed games bz", home);
}

ko c::init(const string& r2rhome, protocol_factories_t& f) {
    name = "games";
    auto r = b::init(r2rhome, f);
    if (is_ko(r)) {
        return r;
    }
    return ok;
}

string c::homedir() const {
    ostringstream os;
    os << r2rhome << '/' << protocol::name;
    log("homedir", os.str());
    return os.str();
}

/*
std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection) {
    log("protocol from string", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != c::protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 30191 unsupported protocol";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "player") {
        auto r = "KO 30192 only allowed role: player ";
        log(r);
        return make_pair(r, nullptr);
    }
    log("creating protocol", c::protocol::name, protocol_selection.second);
    return create_protocol();
}
*/
/*
std::pair<ko, us::wallet::trader::trader_protocol*> c::create_opposite_protocol(protocol_selection_t&& protocol_selection) {
    log("create_opposite_protocol", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 41867 Unsupported protocol.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "player") {
        auto r = "KO 91472 player: I only match player.";
        log(r);
        return make_pair(r, nullptr);
    }
    return create_protocol();
}
*/
std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol() {
    log("protocol instatiation");
    auto p = new business_t::protocol(*this);
    log("protocol instance at", p, 1);
    log("created protocol", c::protocol::name);
    return make_pair(ok, p);
}

void c::list_protocols(ostream& os) const {
    os << c::protocol::name << " player\n";
}

bool c::invert(protocol_selection_t& i) const {  //symmetric role
    if (i.first != protocol::name) return false;
    if (i.second == "player") return true;
    return false;
}

void c::published_protocols(protocols_t& protocols, bool inverse) const {
    protocols.emplace_back(c::protocol::name, "player");
}

void c::exec_help(const string& prefix , ostream& os) const {
    protocol::exec_help(prefix, os);
}

ko c::exec(istream& is, wallet_local_api& w) {
    return protocol::exec(is, w);
}

