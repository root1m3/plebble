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
#include "protocol.h"
#include <us/wallet/protocol.h>

#define loglevel "trader/r2r/bid2ask"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::protocol;

c::protocol(b::business_t& bz): b(bz) {
    log("create bid2ask protocol home=", phome);
}

c::~protocol() {
    log("destroy bid2ask protocol");
}

c::workflow_t::workflow_t(trader_t& tder_, ch_t& ch): tder(tder_) {
    log("constructor workflow");
    inv = enable_invoice(true, ch);
    cat = enable_catalogue(true, ch);
    pay = enable_payment(true, ch);
    rcpt = enable_receipt(true, ch);
}

c::workflow_t::~workflow_t() {
}

c::shipping_receipt_t* c::ship(const shipping_options_t& o, const parcel_t& p) {
    //sell on oxchange
    //send to exchange
    //create_receipt(p);
    return nullptr;
}

void c::post_configure(ch_t& ch) {
    log("post_configure");
    assert(_workflow == nullptr);
    assert(tder != nullptr);
    log("creating workflow");
    _workflow = add(new workflow_t(*tder, ch), ch);
}

ko c::rehome(ch_t& ch) {
    auto r = b::rehome(ch);
    if (is_ko(r)) {
        return r;
    }
    assert(!ch.closed());
    basket.set(ch);
    return ok;
}

ko c::on_attach(trader_t& tder_, ch_t& ch) {
    log("on_attach", "trace-w8i");
    auto r = b::on_attach(tder_, ch);
    if (is_ko(r)) {
        return r;
    }
    post_configure(ch);
    log("on_attach. returned", ch.to_string());
    return move(r);
}

void c::data(const string& lang, ostream& os) const {
    b::data(lang, os);
    os << "basket " << basket.encode() << '\n';
}

void c::help_show(const string& indent, ostream& os) const {
    b::help_show(indent, os);
    twocol(indent, "stock", "Print products in stock", os);
    twocol(indent, "basket", "Print selected products", os);
}

blob_t c::push_payload(uint16_t pc) const {
    if (pc < push_begin) {
        return b::push_payload(pc);
    }
    assert(pc < push_end);
    blob_t blob;
    switch(pc) {
        case push_basket: { //basket
            log("pushing basket");
            basket.write(blob);
        }
        break;
    }
    return move(blob);
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    {
        auto r = b::exec_offline(cmd0, ch);
        if (r != WP_29101) return move(r);
    }
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "show") {
        string cmd;
        is >> cmd;
        if (cmd == "stock") {
            tder->schedule_push(get_push_datagram(push_stock));
            return ok;
        }
        if (cmd == "basket") {
            log("OK - basket will be pushed");
            tder->schedule_push(get_push_datagram(push_basket));
            return ok;
        }
    }
    auto r = WP_29101;
    log(r);
    return r;
}

