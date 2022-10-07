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

#include <us/wallet/trader/workflow/workflows_t.h>

#include "consumer_workflow_t.h"

#define loglevel "trader/r2r/bid2ask"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::protocol;

c::protocol(business_t& bz): b(bz) {
    log("create bid2ask protocol home=", phome);
    using workflows_t = us::wallet::trader::workflow::workflows_t;

    struct my_workflow_factory_t: workflows_t::workflow_factory_t {

        my_workflow_factory_t(c& p): p(p) {}

        pair<ko, value_type*> create() const override {
            auto x = new bid2ask::consumer_workflow_t();
            ch_t ch(0);
            x->init(p._workflows, ch);
            return make_pair(ok, x);
        }

        c& p;
    };
    log("registering workflow factory. object id ", bid2ask::consumer_workflow_t::my_factory_id);
    _workflows.workflow_factories.register_factory(bid2ask::consumer_workflow_t::my_factory_id, new my_workflow_factory_t(*this));
}

c::~protocol() {
    log("destroy bid2ask protocol");
}

c::shipping_receipt_t* c::ship(const shipping_options_t& o, const parcel_t& p) {
    //sell on oxchange
    //send to exchange
    //create_receipt(p);
    return nullptr;
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

void c::create_workflows(ch_t& ch) {
    if (consumer_workflow != nullptr) {
        return;
    }
    log("creating consumer_workflow");
    auto wf = _workflows.workflow_factories.create(consumer_workflow_t::my_factory_id);
    assert(is_ok(wf.first));
    auto wf2 = static_cast<consumer_workflow_t*>(wf.second);
    consumer_workflow = add(wf2, ch);
    assert(consumer_workflow != nullptr);
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

us::gov::io::blob_t c::push_payload(uint16_t pc) const {
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

size_t c::blob_size() const {
    size_t sz = b::blob_size() + basket.blob_size();
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    b::to_blob(writer);
    writer.write(basket);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(basket);
        if (is_ko(r)) {
            return r;
        }
    }
    if (_workflows.size() != 1) {
        auto r = "KO 65092 Invalid number of workflows.";
        log(r, _workflows.size());
        return r;
    }
    consumer_workflow = dynamic_cast<consumer_workflow_t*>(*_workflows.begin());
    if (consumer_workflow == nullptr) {
        auto r = "KO 65093 workflow has wrong type.";
        log(r);
        return r;
    }
    return ok;
}

