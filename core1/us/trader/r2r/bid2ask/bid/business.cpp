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

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>

//#include <us/wallet/wallet/local_api.h>

#define loglevel "trader/r2r/bid2ask/bid"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask::bid;
using c = us::trader::r2r::bid2ask::bid::business_t;
using std::string;
using us::ko;

c::business_t() {
    name = "buyer";
}

c::~business_t() {
    log("destroyed buyer @", home);
}

c::protocol_factory_id_t c::protocol_factory_id() const {
    return protocol_factory_id_t(c::protocol::name, "bid");
}

void c::register_factories(protocol_factories_t& protocol_factories) {
    struct my_protocol_factory_t: protocol_factory_t {

        my_protocol_factory_t(c* bz): bz(bz) {}

        pair<ko, value_type*> create() override {
            auto a = new business_t::protocol(*bz);
            log("created protocol", protocol::name, "instance at", a);
            return make_pair(ok, a);
        }

        c* bz;
    };
    protocol_factories.register_factory(protocol_factory_id(), new my_protocol_factory_t(this));
    assert(protocol_factories.find(protocol_factory_id()) != protocol_factories.end());
}

string c::homedir() const {
    ostringstream os;
    os << r2rhome << '/' << protocol::name << "/bid";
    log("homedir", os.str());
    return os.str();
}

/*
std::pair<us::ko,us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection) {
    log ("protocol from string", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 49867 Unsupported protocol.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "bid") {
        auto r = "KO 95472 buyer: I can only do bid.";
        log(r);
        return make_pair(r, nullptr);
    }
    return create_protocol();
}
*/

std::pair<us::ko,us::wallet::trader::trader_protocol*> c::create_opposite_protocol(protocol_selection_t&& protocol_selection) {
    log ("create_opposite_protocol", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 49867 Unsupported protocol.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "ask") {
        auto r = "KO 95472 buyer: I only match ask.";
        log(r);
        return make_pair(r, nullptr);
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

void c::invert(protocols_t& protocols) const {
    for (auto& i: protocols) {
        if (i.first != c::protocol::name) continue;
        if (i.second == "ask") {
            i.second = "bid";
        }
        else {
            i.second = "ask";
        }
    }
}

void c::published_protocols(protocols_t& protocols, bool inverse) const {
    protocols.emplace_back(c::protocol::name, inverse ? "ask" : "bid");
}

