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

#define loglevel "wallet/trader/w2w"
#define logclass "business"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::r2r::w2w::business_t;

c::business_t() {
    log("constructor w2w bz");
}

c::~business_t() {
    log("destroyed w2w bz", home);
}

c::protocol_factory_id_t c::protocol_factory_id() const {
    return protocol_factory_id_t(c::protocol::name, "w");
}

void c::register_factories(protocol_factories_t& protocol_factories) {
    struct my_protocol_factory_t: protocol_factory_t {

        my_protocol_factory_t(c* bz): bz(bz) {}

        pair<ko, value_type*> create() override {
            auto a = new business_t::protocol(*bz);
            log("created protocol", protocol::name, "instance at", a, 1);
            return make_pair(ok, a);
        }

        c* bz;
    };
    protocol_factories.register_factory(protocol_factory_id(), new my_protocol_factory_t(this));
    assert(protocol_factories.find(protocol_factory_id()) != protocol_factories.end());
}

/*
void c::register_factories(workflow_factories_t& workflow_factories) {
    struct my_workflow_factory_t: workflow_factory_t {
        using b = workflow_factory_t;

        my_workflow_factory_t(const workflow_factory_id_t& fid, c* bz): bz(bz), b(fid) {
        }

        ~my_workflow_factory_t() {
        }

        value_type* create() override {
            return new bid2ask::workflow_t();
        }

        c* bz;
    };

    workflow_factories.register_factory(new my_workflow_factory_t(workflow_factory_id(), this));
    assert(workflow_factories.find(workflow_factory_id()) != workflow_factories.end());
}
*/

ko c::init(const string& r2rhome, us::wallet::trader::traders_t::protocol_factories_t& f) {
    name = "bank";
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
std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol() {
    log("protocol from string", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != c::protocol::name) {
        log("not recognized", protocol_selection.first);
        auto r = "KO 30191 unsupported protocol";
        log(r);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "w") {
        auto r = "KO 30192 only allowed role: w ";
        log(r);
        return make_pair(r, nullptr);
    }
    log("creating protocol", c::protocol::name, protocol_selection.second);
    return create_protocol();
}
*/

std::pair<ko, us::wallet::trader::trader_protocol*> c::create_opposite_protocol(protocol_selection_t&& protocol_selection) {
    if (protocol_selection.first == "w2w") {
        return create_protocol();
    }
    auto r = "KO 30195";
    log(r);
    return make_pair(r, nullptr);
}

std::pair<ko, us::wallet::trader::trader_protocol*> c::create_protocol() {
    log("protocol instatiation");
    auto p = new business_t::protocol(*this);
    log("protocol instance at", p, 1);
    log("created protocol", c::protocol::name);
    return make_pair(ok, p);
}

void c::list_protocols(ostream& os) const {
    os << c::protocol::name << " w\n";
}

void c::invert(protocols_t&) const { //w2w is symmetric
}

void c::published_protocols(protocols_t& protocols, bool inverse) const {
    protocols.emplace_back(protocol_selection_t(c::protocol::name, "w"));
}

void c::exec_help(const string& prefix , ostream& os) const {
    protocol::exec_help(prefix, os);
}

ko c::exec(istream& is, traders_t& traders, wallet::local_api& w) {
    return protocol::exec(is, traders, w);
}

