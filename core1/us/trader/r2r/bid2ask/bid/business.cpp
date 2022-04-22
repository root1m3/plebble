//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/protocol.h>
#include <us/wallet/wallet/local_api.h>

#define loglevel "trader/r2r/bid2ask/bid"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::bid::business_t;

c::business_t() {
}

c::~business_t() {
    log("destroyed buyer @", home);
}

ko c::init(const string& r2rhome) {
    name = "buyer";
    auto r = b::init(r2rhome);
    if (is_ko(r)) {
        return r;
    }
    log("created buyer @", home);
    return ok;
}

string c::homedir() const {
    ostringstream os;
    os << r2rhome << '/' << protocol::name << "/bid";
    log("homedir", os.str());
    return os.str();
}

std::pair<us::ko,us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection) {
    log ("protocol from string", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 49867 Unsupported protocol.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "bid") {
        auto r = "KO 95472 buyer: Unsupported role.";
        log(r);
        return make_pair(r, nullptr );
    }
    return create_protocol();
}

std::pair<us::ko, us::wallet::trader::trader_protocol*> c::create_protocol() {
    auto a = new business_t::protocol(*this);
    log("protocol instance at", a, 1);
    log("created protocol ", protocol::name);
    return make_pair(nullptr, a);
}

void c::list_protocols(ostream& os) const {
    os << c::protocol::name << " bid\n";
}

void c::to_stream_protocols(protocols_t& protocols) const {
    protocols.emplace_back(make_pair(c::protocol::name, "bid"));
}

void c::published_protocols(protocols_t& protocols) const {
    protocols.emplace_back(make_pair(c::protocol::name, "ask"));
}

