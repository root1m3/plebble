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

#define loglevel "trader/r2r/bid2ask"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::business_t;

c::business_t() {
}

c::~business_t() {
}

/*
c::workflow_factory_id_t c::workflow_factory_id() const {
    return workflow_factory_id_t(1);
}

void c::register_factories(workflow_factories_t& workflow_factories) {
    struct my_workflow_factory_t: workflow_factory_t {

        my_workflow_factory_t(c* bz): bz(bz) {}

        pair<ko, value_type*> create() const override {
            auto x = new bid2ask::workflow_t();
            x->init();
            return make_pair(ok, x);
        }

        c* bz;
    };
    workflow_factories.register_factory(workflow_factory_id(), new my_workflow_factory_t(this));
    assert(workflow_factories.find(workflow_factory_id()) != workflow_factories.end());
}
*/

/*
ko c::init(const string& r2rhome, us::wallet::trader::traders_t::protocol_factories_t& f) {
    auto r = b::init(r2rhome, f);
    if (is_ko(r)) {
        return r;
    }


    

    return ok;
}
*/

