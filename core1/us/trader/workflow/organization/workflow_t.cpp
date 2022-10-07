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
#include "workflow_t.h"
#include "docs.h"

#define loglevel "trader/workflow/organization"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow::organization;
using c = us::trader::workflow::organization::workflow_t;

c::workflow_t() {
}

c::~workflow_t() {
}

c::bitem* c::enable_appointment(bool b, ch_t& ch) {
    return b ? add<appointment_t, 1>(ch) : remove<appointment_t>(ch);
}


c::bitem* c::enable_reference(bool b, ch_t& ch) {
    return b ? add<reference_t, 2>(ch) : remove<reference_t>(ch);
}

namespace {

    using namespace std;
    using namespace us;

    template<typename a, c::item_factory_id_t item_factory_id>
    struct my_item_factory_t: c::item_factory_t {
        pair<ko, value_type*> create() const override {
            return make_pair(ok, new item_t<a, item_factory_id>());
        }
    };

}

void c::register_factories(item_factories_t& item_factories) const {
    item_factories.register_factory(appointment_factory_id, new my_item_factory_t<appointment_t, appointment_factory_id>());
    item_factories.register_factory(reference_factory_id, new my_item_factory_t<reference_t, reference_factory_id>());
}

