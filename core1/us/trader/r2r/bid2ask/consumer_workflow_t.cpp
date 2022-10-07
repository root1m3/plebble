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
#include "consumer_workflow_t.h"

#define loglevel "trader/r2r/bid2ask"
#define logclass "workflow_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::consumer_workflow_t;

c::consumer_workflow_t() {
    log("constructor workflow");
}

c::~consumer_workflow_t() {
}

void c::init2(ch_t& ch) {
    log("init2");
    cat = enable_catalogue(true, ch);
    assert(cat != nullptr);

    inv = enable_invoice(true, ch);
    assert(inv != nullptr);

    pay = enable_payment(true, ch);
    assert(pay != nullptr);

    rcpt = enable_receipt(true, ch);
    assert(rcpt != nullptr);
}

void c::init_bid(ch_t& ch) {
    init2(ch);
    cat->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
    inv->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
    pay->set_mode(us::wallet::trader::workflow::item_t::mode_send, ch);
    rcpt->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
}

void c::init_ask(ch_t& ch) {
    init2(ch);
    cat->set_mode(us::wallet::trader::workflow::item_t::mode_send, ch);
    inv->set_mode(us::wallet::trader::workflow::item_t::mode_send, ch);
    pay->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
    rcpt->set_mode(us::wallet::trader::workflow::item_t::mode_send, ch);
}

size_t c::blob_size() const {
    auto sz = b::blob_size();
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    b::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    ch_t ch(0);
    init2(ch);
    return ok;
}

