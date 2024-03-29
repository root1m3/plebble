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

#define loglevel "trader/workflow/logistics"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow::logistics;
using c = us::trader::workflow::logistics::workflow_t;

c::workflow_t() {
}

c::~workflow_t() {
}

c::bitem* c::enable_parcel(bool b, ch_t& ch) {
    return b ? add<parcel_t, 1>(ch) : remove<parcel_t>(ch);
}

c::bitem* c::enable_shipping_receipt(bool b, ch_t& ch) {
    return b ? add<shipping_receipt_t, 2>(ch) : remove<shipping_receipt_t>(ch);
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
    item_factories.register_factory(parcel_factory_id, new my_item_factory_t<parcel_t, parcel_factory_id>());
    item_factories.register_factory(shipping_receipt_factory_id, new my_item_factory_t<shipping_receipt_t, shipping_receipt_factory_id>());
}

