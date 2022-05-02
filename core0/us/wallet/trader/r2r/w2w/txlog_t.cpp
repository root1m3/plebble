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
#include "txlog_t.h"
#include "protocol.h"

#define loglevel "wallet/trader/r2r/w2w"
#define logclass "txlog_t"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::r2r::w2w::txlog_t;

r2r::w2w::track_t c::store_new_invoice(const string& io_summary, uint16_t state_id, const string& state, tx_t* tx) {
    log("store_new_invoice", io_summary, tx->ts);
    lock_guard<mutex> lock(mx);
    emplace(tx->ts, txlog_item_t(io_summary, state_id, state, tx, nullptr));
    if (p) p->schedule_push(r2r::w2w::protocol::push_txlog);
    return tx->ts;
}

r2r::w2w::track_t c::store_recv_invoice(const string& io_summary, uint16_t state_id, const string& state, tx_t* tx) {
    log("store_recv_invoice", io_summary, tx->ts);
    lock_guard<mutex> lock(mx);
    emplace(tx->ts, txlog_item_t(io_summary, state_id, state, tx, nullptr));
    if (p) p->schedule_push(r2r::w2w::protocol::push_txlog);
    return tx->ts;
}

r2r::w2w::track_t c::store_new_tx(const track_t& invoice_track_id, const string& io_summary, uint16_t state_id, const string& state, tx_t* tx) {
    log("store_new_tx", invoice_track_id, io_summary, tx->ts);
    lock_guard<mutex> lock(mx);
    auto i = find(invoice_track_id);
    if (i == end()) {
        log("invoice not found in txlog with track id", invoice_track_id);
        i = emplace(tx->ts, txlog_item_t(io_summary, state_id, state, nullptr, nullptr)).first;
    }
    i->second.pay.reset(tx);
    i->second.io_summary = io_summary;
    i->second.state_id = state_id;
    i->second.state = state;
    if (p) p->schedule_push(r2r::w2w::protocol::push_txlog);
    return i->first;
}

r2r::w2w::index_t c::index() const {
    log("index");
    index_t r;
    r.reserve(size());
    for (auto& i: *this) {
        r.emplace_back(make_pair(i.first, index_item_t(i.first, i.second.title(), i.second.state_id)));
    }
    return move(r);
}

void c::dump(const string& prefix, ostream& os) const {
    lock_guard<mutex> lock(mx);
    for (auto& i: *this) {
        os << prefix << i.first << ' ' << i.second.title() << '\n';
    }
}

ko c::cancel(const track_t& track) {
    lock_guard<mutex> lock(mx);
    auto i = find(track);
    if (i == end()) {
        return "KO 63092 track_id not found.";
    }
    erase(i);
    if (p) p->schedule_push(r2r::w2w::protocol::push_txlog);
    return ok;
}

pair<ko, us::gov::cash::tx_t*> c::get_invoice(const track_t& track) const {
    lock_guard<mutex> lock(mx);
    auto i = find(track);
    if (i == end()) {
        auto r = "KO 63092 track_id not found.";
        log(r);
        return make_pair(r, nullptr);
    }
    if (!i->second.inv) {
        auto r = "KO 63732 invoice not available.";
        log(r);
        return make_pair(r, nullptr);
    }
    return make_pair(ok, new tx_t(*i->second.inv));
}


void us::wallet::trader::r2r::w2w::index_t::dump(const string& prefix, ostream& os) const {
    for (auto& i: *this) {
        os << prefix << i.first << ' ' << i.second.label << '\n';
    }
}

size_t us::wallet::trader::r2r::w2w::index_item_t::blob_size() const {
    return blob_writer_t::blob_size(track) + blob_writer_t::blob_size(label) + blob_writer_t::blob_size(state);
}

void us::wallet::trader::r2r::w2w::index_item_t::to_blob(blob_writer_t& writer) const {
    writer.write(track);
    writer.write(label);
    writer.write(state);
}

ko us::wallet::trader::r2r::w2w::index_item_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(track);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(label);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(state);
        if (is_ko(r)) return r;
    }
    return ok;
}


