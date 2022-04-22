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
#include "protocol.h"

#include <fstream>

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/engine/daemon_t.h>

#include <us/wallet/protocol.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/trader/trader_t.h>
#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#include "business.h"

#define loglevel "wallet/trader/r2r/w2w"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::r2r::w2w::protocol;

const char* c::rolestr() const { return "w"; }
const char* c::peer_rolestr() const { return "w"; }
const char* c::get_name() const { return name; }

c::protocol(business_t& bz): b(bz), txlog(*this) {
    log("created protocol_w2w_w");
}

c::~protocol() {
    log("destroyed protocol_w2w_w");
}

bool c::requires_online(const string& cmd) const {
    if (b::requires_online(cmd)) return true;
    if (cmd == "transfer") return true;
    if (cmd == "pay") return true;
    if (cmd == "cancel") return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    b::help_online(indent, os);
    os << indent << "transfer <amount> <coin|gas>                        Send funds to peer.\n";
    os << indent << "pay <txlog_id>                                      Commit payment.\n";
    os << indent << "cancel <txlog_id>                                   Cancel payment.\n";
}

void c::help_onoffline(const string& indent, ostream& os) const {
    b::help_onoffline(indent, os);
    os << indent << "track <txlog_id>                                    Track tx status in the txlog.\n";
}

void c::help_show(const string& indent, ostream& os) const {
    b::help_show(indent, os);
    os << indent << "txlog                                               Show txlog.\n";
}

blob_t c::push_payload(uint16_t pc) const {
    log("push_payload. code", pc);
    assert((uint16_t)push_begin >= (uint16_t)trader_protocol::push_end);
    assert(pc < push_end);
    if (pc < push_begin) {
        return b::push_payload(pc);
    }
    blob_t blob;
    switch(pc) {
        case push_tx: {
        }
        break;
        case push_txlog: {
            log("computing push_txlog");
            txlog.index().write(blob);
        }
        break;
        default:
            assert(false);
        break;
    }
    return move(blob);
}

ko c::exec_offline(const string& cmd0, ch_t& ch) {
    {
        auto r = b::exec_offline(cmd0, ch);
        if (r != trader_protocol::WP_29101) return move(r);
    }
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "show") {
        string cmd;
        is >> cmd;
        if (cmd == "txlog") {
            tder->schedule_push(get_push_datagram(push_txlog));
            return ok;
        }
    }
    if (cmd == "track") { //trackid
        log("track");
        track_t track;
        is >> track;
        if (is.fail()) {
            auto r = "KO 65899 Invalid track_id.";
            log(r);
            return move(r);
        }
        string progress;
        auto r = w().track(track, progress);
        if (is_ko(r)) {
            return r;
        }
        return tder->push_OK(progress);
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

c::t2_t::t2_t(const track_t& track, tx_t* tx): track(track), tx(tx) {
}

size_t c::t2_t::blob_size() const {
    return blob_writer_t::blob_size(track) + blob_writer_t::blob_size(*tx);
}

void c::t2_t::to_blob(blob_writer_t& writer) const {
    writer.write(track);
    writer.write(*tx);
}

ko c::t2_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(track);
        if (is_ko(r)) return r;
    }
    {
        tx_t* t = new tx_t();
        auto r = reader.read(*t);
        if (is_ko(r)) {
            delete t;
            return r;
        }
        delete tx;
        tx = t;
    }
    return ok;
}

c::t1_t::t1_t(const cash_t& amount, const hash_t& coin): amount(amount), coin(coin) {
}

size_t c::t1_t::blob_size() const {
    return blob_writer_t::blob_size(amount) + blob_writer_t::blob_size(coin);
}

void c::t1_t::to_blob(blob_writer_t& writer) const {
    writer.write(amount);
    writer.write(coin);
}

ko c::t1_t::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(amount);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(coin);
        if (is_ko(r)) return r;
    }
    return ok;
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    log("exec_online", cmd0);
    {
        auto r = b::exec_online(peer, cmd0, ch);
        if (r != WP_29101) return move(r);
    }
    log("exec_online", "here", cmd0);
    istringstream is(cmd0);
    string cmd;
    is >> cmd;
    if (cmd == "transfer") {
        log("transfer");
        t1_t t1(0, hash_t(0));
        is >> t1.amount;
        if (is.fail()) {
            auto r = "KO 30203 Invalid amount.";
            log(r);
            return move(r);
        }
        if (t1.amount == 0) {
            auto r = "KO 30204 Invalid amount.";
            log(r);
            return move(r);
        }
        t1.coin = us::gov::io::shell_args::next_token(is);
        if (is.fail()) {
            auto r = "KO 30205 Invalid coin.";
            log(r);
            return move(r);
        }
        blob_t blob;
        t1.write(blob);
        log("send peer svc_invoice_query");
        auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_invoice_query, blob));
        if (is_ko(r)) {
            return move(r);
        }
        return ok;
    }
    if (cmd == "pay") { //trackid
        log("pay");
        track_t track;
        is >> track;
        if (is.fail()) {
            auto r = "KO 65899 Invalid track_id.";
            log(r);
            return move(r);
        }
        //TODO verify outputs. must match stored amount+coin
        log("RBF", "check outputs");
        tx_t* tx;
        {
            pair<ko, tx_t*> r = txlog.get_invoice(track);
            if (is_ko(r.first)) {
                return r.first;
            }
            assert(r.second != nullptr);
            tx = r.second;
        }

        {
            string pg_info;
            using affected_t = us::wallet::wallet::algorithm::affected_t;
            pair<ko, affected_t> r = w().track_pay("algS0", "algR0", *tx, pg_info);
            if (is_ko(r.first)) {
                delete tx;
                return r.first;
            }
            txlog.store_new_tx(track, r.second.to_string(), 4, pg_info, tx);
        }
        log("paid tx", tx->encode());
        {
            blob_t response_blob;
            {
                t2_t t2(track, tx);
                t2.write(response_blob);
            }
            log("Forward to peer. track", track);
            auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_tx, response_blob));
            if (is_ko(r)) {
                return r;
            }
            log("sent signed tx to peer"); //, os.str());
        }
        return ok;

    }
    if (cmd == "cancel") { //trackid
        log("cancel");
        track_t track;
        is >> track;
        if (is.fail()) {
            auto r = "KO 65899 Invalid track_id.";
            log(r);
            return move(r);
        }
        auto r = txlog.cancel(track);
        if (is_ko(r)) {
            return r;
        }
        blob_t blob;
        blob_writer_t w(blob, blob_writer_t::blob_size(track));
        w.write(track);
        return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_cancel, blob));
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

void c::exec_help(const string& prefix , ostream& os) {
    b::exec_help(prefix + "basic ", os);
    os << prefix << "info\n";
}

ko c::exec(istream& is, traders_t& traders, wallet::local_api& w) {
    string cmd;
    is >> cmd;
    if (cmd == "info") {
        return traders.push_OK("info TBD.", w);
    }
    if (cmd == "basic") {
        return b::exec(is, traders, w);
    }
    auto r = "KO 10918 Invalid command";
    log(r);
    return r;
}

#include <us/gov/io/cfg0.h>

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    log("trading_msg");
    static_assert( (uint16_t)svc_begin >= (uint16_t)trader_protocol::svc_end);
    assert(svc < svc_end);
    if (svc < svc_begin) {
        return b::trading_msg(peer, svc, move(blob));
    }
    switch(svc) {
        case svc_invoice_query: {
        log("received svc_invoice_query");
            log("invoice_request");
            t1_t t1(0, hash_t(0));
            auto r = t1.read(blob);
            if (is_ko(r)) {
                return r;
            }
            {
                auto r = w().refresh_data();
                if (is_ko(r)) {
                    return r;
                }
            }
            pair<ko, hash_t> ret = w().select_receiving_account();
            if (is_ko(ret.first)) {
                return ret.first;
            }
            hash_t recv_address = ret.second;
            if (recv_address.is_zero()) {
                ko e = "KO 22019 Invalid recv address";
                log(e);
                return e;
            }
            tx_t* tx;
            {
                auto r = w().tx_charge(recv_address, t1.amount, t1.coin);
                if (is_ko(r.first)) {
                    return ret.first;
                }
                tx = r.second;
            }
            assert(tx != nullptr);
            using affected_t = us::wallet::wallet::algorithm::affected_t;
            affected_t a;
            a.add_charge(t1.amount, t1.coin);
            track_t track = txlog.store_new_invoice(a.to_string(), 1, "Waiting for peer confirmation.", tx);
            blob_t response_blob;
            tx->write(response_blob);
            {
                auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_invoice, response_blob));
                if (is_ko(r)) {
                    return r;
                }
            }
            return ok;
        }
        break;
        case svc_invoice: {
            log("received svc_invoice");
            log("received outputs");
            cash::tx_t* inv = new cash::tx_t();
            {
                auto r = inv->read(blob);
                if (is_ko(r)) {
                    delete inv;
                    return r;
                }
            }
            {
                using affected_t = us::wallet::wallet::algorithm::affected_t;
                pair<ko, affected_t> r = w().analyze_pay("algS0", "algR0", *inv);
                track_t inv_track = txlog.store_recv_invoice(r.second.to_string(), 2, "Confirm payment.", inv);
            }
            return ok;
        }
        break;
        case svc_cancel: {
            log("received svc_tx");
            track_t track;
            {
                blob_reader_t reader(blob);
                auto r = reader.read(track);
                if (is_ko(r)) {
                    return r;
                }
            }
            return txlog.cancel(track);
        }
        break;
        case svc_tx: {
            log("received svc_tx");
            tx_t* tx;
            track_t track;
            {
                t2_t t2;
                auto r = t2.read(blob);
                if (is_ko(r)) {
                    auto r = "KO 92201 Invalid transaction wrapper t2.";
                    log(r);
                    return r;
                }
                track = t2.track;
                tx = t2.tx;
            }
            log("Received peer's payment. track", track);

            log("relay tx + track");
            string pg_info;
            {
                log("Broadcasting tx. track", track, "tx.ts=", tx->ts);
                blob_t blobev;
                tx->write(blobev);
                auto r = w().daemon.gov_rpc_daemon.get_peer().call_ev_track(blobev, pg_info);
                if (is_ko(r)) {
                    delete tx;
                    return r;
                }
            }
            txlog.store_new_tx(track, "tx-notrace", 4, pg_info, tx);
            return ok;
        }
    }
    return trader_protocol::KO_29100;
}

void c::dump(ostream& os) const {
    os << "w2w w";
}

void c::list_trades_bit(ostream& os) const {
    os << "w2w w ";
}

namespace { namespace i18n {

    struct r_en_t;
    struct r_es_t;

    struct r_t: unordered_map<uint32_t, const char*> {
        using b =  unordered_map<uint32_t, const char*>;

        using b::unordered_map;

        const char* resolve(uint32_t n) const {
            log("string-res. resolve", n);
            auto i = find(n);
            if (i == end()) return begin()->second;
            return i->second;
        }

        static r_t& resolver(const string& lang);
    };

    struct r_en_t: r_t {
        using b = r_t;

        r_en_t(): b({
            {0, "KO 30920 Use i18n package in Lower Layer."},
            {1, "Transfer funds to peer."}, {2, "Input coin and amount."},

        }) {
            //log("constructor 'en' string resources with", size(), "entries. Entry #4 is", resolve(4));
        }

    };

    struct r_es_t: r_t {
        using b = r_t;

        r_es_t(): b({
            {0, "KO 30920"},
            {1, "Transferencia."}, {2, "Introduzca moneda y cantidad."},

        }) {
            //log("constructor 'es' string resources with", size(), "entries. Entry #4 is", resolve(4));
        }

    };

    r_en_t r_en;
    r_es_t r_es;

    r_t& r_t::resolver(const string& lang) {
        if (lang == "es") return r_es;
        return r_en;
    }

}}

uint32_t c::trade_state_() const {
    log("trade_state");
    return 1;
}

void c::judge(const string& lang) {
    auto st = trade_state_();
    if (st != _trade_state.first) {
        if (st == 0) {
            b::judge(lang);
            return;
        }
        auto t = i18n::r_t::resolver(lang);
        auto r = t.resolve(st);
        log("trade_state", st, r);
        _trade_state = make_pair(st, r);
        _user_hint = t.resolve(_trade_state.first + 1);
    }
}

