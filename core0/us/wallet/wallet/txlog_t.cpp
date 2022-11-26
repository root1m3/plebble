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

#include <chrono>

#include <us/gov/engine/track_status_t.h>

#include <us/wallet/engine/daemon_t.h>

#include "local_api.h"
#include "txlog_item_t.h"

#define loglevel "wallet/wallet"
#define logclass "txlog_t"
#include <us/gov/logs.inc>

using namespace us::wallet::wallet;
using c = us::wallet::wallet::txlog_t;

track_t c::register_transfer(const cash_t& amount, const hash_t& coin, const trade_id_t& trade_id, blob_t& blob) {
    log("register_transfer. payer.");
    auto t1 = new t1_t(amount, coin);
    using namespace std::chrono;
    track_t track = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    {
        blob_writer_t w(blob, blob_writer_t::blob_size(track) + t1->blob_size());
        w.write(track);
        w.write(*t1);
    }
    {
        lock_guard<mutex> lock(mx);
        emplace(track, txlog_item_t(t1, true, trade_id));
    }
    if (w) {
        log("pushing push_txlog1");
        w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
    }
    return track;
}

ko c::register_transfer(const blob_t& blob, const trade_id_t& trade_id, blob_t& response_blob) {
    log("register_transfer. payee.");
    track_t track;
    blob_reader_t reader(blob);
    {
        auto r = reader.read(track);
        if (is_ko(r)) {
            return r;
        }
    }
    iterator i;
    {
        lock_guard<mutex> lock(mx);
        i = find(track);
        if (i != end()) {
            auto r = "KO 23091 track_id already exists.";
            log(r);
            return r;
        }
        auto* t1 = new t1_t();
        {
            auto r = reader.read(*t1);
            if (is_ko(r)) {
                delete t1;
                return r;
            }
        }
        i = emplace(track, txlog_item_t(t1, false, trade_id)).first;
    }
    assert(w != nullptr);
    log("pushing push_txlog2");
    w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
    {
        auto r = w->refresh_data();
        if (is_ko(r)) {
            return r;
        }
    }
    hash_t recv_address;
    {
        pair<ko, hash_t> ret = w->select_receiving_account();
        if (is_ko(ret.first)) {
            return ret.first;
        }
        recv_address = ret.second;
        if (recv_address.is_zero()) {
            ko e = "KO 22019 Invalid recv address";
            log(e);
            return e;
        }
    }
    tx_t* inv;
    {
        auto r = w->tx_charge(recv_address, i->second.t1->amount, i->second.t1->coin);
        if (is_ko(r.first)) {
            return r.first;
        }
        inv = r.second;
    }
    assert(inv != nullptr);
    {
        blob_writer_t w(response_blob, blob_writer_t::blob_size(track) + inv->blob_size());
        w.write(track);
        w.write(*inv); //tx->write(response_blob);
    }
    {
        lock_guard<mutex> lock(mx);
        i->second.set_inv(inv);
    }
    log("pushing push_txlog3");
    w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
    return ok;
}

ko c::register_invoice(const blob_t& blob) {
    log("register_invoice. payer.");
    assert(w != nullptr);
    track_t track;
    blob_reader_t reader(blob);
    {
        auto r = reader.read(track);
        if (is_ko(r)) {
            return r;
        }
    }
    cash::tx_t* inv = new cash::tx_t();
    {
        auto r = reader.read(*inv); //inv->read(blob);
        if (is_ko(r)) {
            delete inv;
            return r;
        }
    }
    string affected;
    {
        using affected_t = us::wallet::wallet::algorithm::affected_t;
        pair<ko, affected_t> r = w->analyze_pay("algS0", "algR0", *inv);
        if (is_ko(r.first)) {
            delete inv;
            return r.first;
        }
        affected = r.second.to_string("  ");
    }
    trade_id_t tid;
    {
        lock_guard<mutex> lock(mx);
        auto i = find(track);
        if (i == end()) {
            delete inv;
            auto r = "KO 10091 track_id not found.";
            log(r);
            return r;
        }
        i->second.set_inv(inv);
        i->second.io_summary += "\ninvoice:\n" + affected;
        tid = i->second.trade_id;
    }
    log("pushing push_txlog4");
    w->schedule_push(w->get_push_datagram(tid, local_api::push_txlog));
    return ok;
}

ko c::pay_inv(track_t track, blob_t& blob) {
    log("pay_inv", track);
    iterator i;
    trade_id_t trade_id;
    {
        lock_guard<mutex> lock(mx);
        i = find(track);
        if (i == end()) {
            auto r = "KO 11091 track_id not found.";
            log(r);
            return r;
    //        i = emplace(tx->ts, txlog_item_t(io_summary, move(st), nullptr, nullptr)).first;
        }
        trade_id = i->second.trade_id;
    }
    //TODO verify outputs. must match stored amount+coin
    log("RBF", "check outputs");
    tx_t* tx;
    {
        pair<ko, tx_t*> r = get_invoice(track);
        if (is_ko(r.first)) {
            return r.first;
        }
        assert(r.second != nullptr);
        tx = r.second;
    }
    {
        using affected_t = us::wallet::wallet::algorithm::affected_t;
        gov_track_status_t gov_track_status;
        pair<ko, affected_t> r = w->track_pay("algS0", "algR0", *tx, trade_id, gov_track_status);
        if (is_ko(r.first)) {
            delete tx;
            return r.first;
        }
        lock_guard<mutex> lock(mx);
        i->second.pay.reset(tx);
        i->second.io_summary = r.second.to_string("");
        i->second.wallet_evt_status = wevt_delivered;
        i->second.gov_evt_status = gov_track_status.st;
        i->second.gov_evt_status_info = gov_track_status.info;
    }
    log("paid tx", tx->encode());
    {
        blob_writer_t w(blob, blob_writer_t::blob_size(track) + tx->blob_size());
        w.write(track);
        w.write(*tx); //tx->write(response_blob);
    }
    log("pushing push_txlog5");
    w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
    return ok;
}

ko c::register_tx(const blob_t& blob) {
    log("register_tx. payee.");
    assert(w != nullptr);

    track_t track;
    blob_reader_t reader(blob);
    {
        auto r = reader.read(track);
        if (is_ko(r)) {
            return r;
        }
    }
    tx_t* tx = new tx_t();
    {
        auto r = reader.read(*tx);
        if (is_ko(r)) {
            auto r = "KO 92201 Invalid tx.";
            log(r);
            return r;
        }
    }
    {
        ostringstream os;
        auto r = tx->verify(os);
        if (!r) {
            delete tx;
            auto r = "KO 92101 Invalid tx.";
            log(r, os.str());
            return r;
        }
    }
    log("relay tx + track");
    gov_track_status_t gov_track_status;
    {
        log("Broadcasting tx. track", track, "tx.ts=", tx->ts);
        blob_t blobev;
        tx->write(blobev);
        auto r = w->daemon.gov_rpc_daemon.get_peer().call_ev_track(blobev, gov_track_status);
        if (is_ko(r)) {
            gov_track_status.st = us::gov::engine::evt_unknown;
            gov_track_status.info = r;
        }
    }
    log("Received peer's payment. track", track);
    trade_id_t tid;
    {
        lock_guard<mutex> lock(mx);
        auto i = find(track);
        if (i == end()) {
            delete tx;
            auto r = "KO 19091 track_id not found.";
            log(r);
            return r;
        }
        i->second.set_tx(tx);
        i->second.io_summary += "\nReceived tx.\n";
        i->second.wallet_evt_status = wevt_delivered;
        i->second.gov_evt_status = gov_track_status.st;
        i->second.gov_evt_status_info = gov_track_status.info;
        tid = i->second.trade_id;
    }
    log("pushing push_txlog6");
    w->schedule_push(w->get_push_datagram(tid, local_api::push_txlog));
    return ok;
}

index_t c::index() const {
    log("index");
    index_t r;
    r.reserve(size());
    for (auto& i: *this) {
        r.emplace_back(make_pair(i.first, index_item_t(i.second.title(), i.second.gov_evt_status, i.second.gov_evt_status_info, i.second.wallet_evt_status)));
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
    trade_id_t trade_id;
    {
        lock_guard<mutex> lock(mx);
        auto i = find(track);
        if (i == end()) {
            auto r = "KO 63052 track_id not found.";
            log(r);
            return r;
        }
        if (i->second.wallet_evt_status == wevt_delivered) {
            auto r = "KO 63051 Already delivered.";
            log(r);
            return r;
        }
        trade_id = i->second.trade_id;
        if (i->second.wallet_evt_status == wevt_cancelled) {
            erase(i);
        }
        else {
            i->second.wallet_evt_status = wevt_cancelled;
        }
    }
    log("pushing push_txlog7");
    w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
    return ok;
}

ko c::cancel(const track_t& track, blob_t& blob) {
    {
        auto r = cancel(track);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        blob_writer_t w(blob, blob_writer_t::blob_size(track));
        w.write(track);
    }
    return ok;
}

ko c::cancel(const blob_t& blob) {
    track_t track;
    {
        blob_reader_t reader(blob);
        auto r = reader.read(track);
        if (is_ko(r)) {
            return r;
        }
    }
    return cancel(track);
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

void c::on_tx_tracking_status(const gov_track_status_t& status) {
    log("on_tx_tracking_status", size());
    set<trade_id_t> notify;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: *this) {
            i.second.on_tx_tracking_status(status, notify);
        }
    }
    if (!notify.empty()) {
        log("pushing push_txlog8");
        w->schedule_push(w->get_push_datagrams(notify, local_api::push_txlog));
    }
}

void c::show(const trade_id_t& trade_id) const {
    log("pushing push_txlog9");
    w->schedule_push(w->get_push_datagram(trade_id, local_api::push_txlog));
}

void index_t::dump(const string& prefix, ostream& os) const {
    auto pfx = prefix + "    ";
    for (auto& i: *this) {
        os << prefix << i.first << ' ';
        i.second.dump(pfx, os);
    }
}

void index_item_t::dump(const string& prefix, ostream& os) const {
    os << label << '\n';
    os << prefix << "wallet-status " << +wallet_evt_status << ' ' << wallet_evt_status_str[wallet_evt_status] << '\n';
    if (wallet_evt_status == wevt_delivered) {
        os << prefix << "gov-status " << +gov_evt_status << ' ' << us::gov::engine::evt_status_str[gov_evt_status];
        if (!gov_evt_status_info.empty()) os << ' ' << gov_evt_status_info;
        os << '\n';
    }
}

size_t index_item_t::blob_size() const {
    return blob_writer_t::blob_size(label) +
        blob_writer_t::blob_size((uint8_t)wallet_evt_status) +
        blob_writer_t::blob_size((uint8_t)gov_evt_status) +
        blob_writer_t::blob_size(gov_evt_status_info);
}

void index_item_t::to_blob(blob_writer_t& writer) const {
    writer.write(label);
    writer.write((uint8_t)wallet_evt_status);
    writer.write((uint8_t)gov_evt_status);
    writer.write(gov_evt_status_info);
}

ko index_item_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(label);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read((uint8_t&)wallet_evt_status);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read((uint8_t&)gov_evt_status);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(gov_evt_status_info);
        if (is_ko(r)) return r;
    }
    return ok;
}

