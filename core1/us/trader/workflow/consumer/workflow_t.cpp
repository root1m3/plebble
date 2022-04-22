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
#include "workflow_t.h"
#include "docs.h"

#define loglevel "trader/workflow/consumer"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow;
using c = us::trader::workflow::consumer::workflow_t;

c::workflow_t() {
}

c::~workflow_t() {
}

c::bitem* c::enable_catalogue(bool b, ch_t& ch) {
    log("enable_catalogue", b);
    return b ? add<catalogue_t>(ch) : remove<catalogue_t>(ch);
}

c::bitem* c::enable_invoice(bool b, ch_t& ch) {
    log("enable_invoice", b);
    return b ? add<invoice_t>(ch) : remove<invoice_t>(ch);
}

c::bitem* c::enable_payment(bool b, ch_t& ch) {
    log("enable_payment", b);
    return b ? add<payment_t>(ch) : remove<payment_t>(ch);
}

c::bitem* c::enable_receipt(bool b, ch_t& ch) {
    log("enable_receipt", b);
    return b ? add<receipt_t>(ch) : remove<receipt_t>(ch);
}

void c::workflow_t::exec_help(const string& prefix, ostream& os) {
    os << prefix << "info\n";
    catalogue_t::exec_help(prefix, os);
    invoice_t::exec_help(prefix, os);
    payment_t::exec_help(prefix, os);
    receipt_t::exec_help(prefix, os);
}

ko c::workflow_t::exec(istream& is, traders_t& traders, wallet::wallet::local_api& w) {
    string cmd;
    is >> cmd;
    if (cmd == "info") {
        return traders.push_OK("I can't find any particularly interesting info.", w);
    }
    if (cmd == catalogue_t::name) {
        return catalogue_t::exec(is, traders, w);
    }
    if (cmd == invoice_t::name) {
        return invoice_t::exec(is, traders, w);
    }
    if (cmd == payment_t::name) {
        return payment_t::exec(is, traders, w);
    }
    if (cmd == receipt_t::name) {
        return receipt_t::exec(is, traders, w);
    }
    return "KO 20908 Invalid command";
}

