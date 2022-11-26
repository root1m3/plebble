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

#include <us/gov/ko.h>

#define loglevel "trader/workflow/consumer"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow::consumer;
using c = us::trader::workflow::consumer::workflow_t;
using ko = us::ko;

c::workflow_t() {
}

c::~workflow_t() {
}

c::bitem* c::enable_catalogue(bool b, ch_t& ch) {
    log("enable_catalogue", b);
    return b ? add<catalogue_t, cat_factory_id>(ch) : remove<catalogue_t>(ch);
}

c::bitem* c::enable_invoice(bool b, ch_t& ch) {
    log("enable_invoice", b);
    return b ? add<invoice_t, inv_factory_id>(ch) : remove<invoice_t>(ch);
}

c::bitem* c::enable_payment(bool b, ch_t& ch) {
    log("enable_payment", b);
    return b ? add<payment_t, pay_factory_id>(ch) : remove<payment_t>(ch);
}

c::bitem* c::enable_receipt(bool b, ch_t& ch) {
    log("enable_receipt", b);
    return b ? add<receipt_t, rcpt_factory_id>(ch) : remove<receipt_t>(ch);
}

void c::workflow_t::exec_help(const string& prefix, ostream& os) {
    os << prefix << "info\n";
    catalogue_t::exec_help(prefix, os);
    invoice_t::exec_help(prefix, os);
    payment_t::exec_help(prefix, os);
    receipt_t::exec_help(prefix, os);
}

ko c::workflow_t::exec(istream& is, wallet::wallet::local_api& w) {
    string cmd;
    is >> cmd;
    if (cmd == "info") {
        return w.push_OK("I can't find any particularly interesting info.");
    }
    if (cmd == catalogue_t::name) {
        return catalogue_t::exec(is, w);
    }
    if (cmd == invoice_t::name) {
        return invoice_t::exec(is, w);
    }
    if (cmd == payment_t::name) {
        return payment_t::exec(is, w);
    }
    if (cmd == receipt_t::name) {
        return receipt_t::exec(is, w);
    }
    return "KO 20908 Invalid command";
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
    item_factories.register_factory(cat_factory_id, new my_item_factory_t<catalogue_t, cat_factory_id>());
    item_factories.register_factory(inv_factory_id, new my_item_factory_t<invoice_t, inv_factory_id>());
    item_factories.register_factory(pay_factory_id, new my_item_factory_t<payment_t, pay_factory_id>());
    item_factories.register_factory(rcpt_factory_id, new my_item_factory_t<receipt_t, rcpt_factory_id>());
}


