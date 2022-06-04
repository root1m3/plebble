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

#include <fstream>

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/engine/daemon_t.h>
#include <us/gov/cli/rpc_peer_t.h>

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

c::protocol(business_t& bz): b(bz) {
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
//    os << indent << "track <txlog_id>                                    Track tx status in the txlog.\n";
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
    assert(false);
    blob_t blob;
    switch(pc) {
        case push_tx: {
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
        log("show");
        string cmd;
        is >> cmd;
        if (cmd == "txlog") {
            log("show txlog");
            w().txlog.show(tid());
            return ok;
        }
    }
/*
    if (cmd == "track") { //trackid
        log("track");
        track_t track;
        is >> track;
        if (is.fail()) {
            auto r = "KO 65899 Invalid track_id.";
            log(r);
            return move(r);
        }
        track_status_t track_status;
        auto r = w().track(track, tder->id, track_status);
        if (is_ko(r)) {
            return r;
        }
        ostringstream os;
        track_status.dump(os);
        return tder->push_OK(os.str());
    }
*/
    auto r = WP_29101;
    log(r);
    return move(r);
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
        cash_t amount{0};
        hash_t coin{0};
        is >> amount;
        if (is.fail()) {
            auto r = "KO 30203 Invalid amount.";
            log(r);
            return move(r);
        }
        if (amount == 0) {
            auto r = "KO 30204 Invalid amount.";
            log(r);
            return move(r);
        }
        coin = us::gov::io::shell_args::next_token(is);
        if (is.fail()) {
            auto r = "KO 30205 Invalid coin.";
            log(r);
            return move(r);
        }
        blob_t blob;
        track_t track = w().txlog.register_transfer(amount, coin, tid(), blob);
        log("send peer svc_invoice_query");
        auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_invoice_query, blob));
        if (is_ko(r)) {
            w().txlog.cancel(track);
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
        blob_t response_blob;
        {
            auto r = w().txlog.pay_inv(track, response_blob);
            if (is_ko(r)) {
                return r;
            }
        }
        log("Forward to peer. track", track);
        auto r = peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_tx, response_blob));
        if (is_ko(r)) {
            return r;
        }
        log("sent signed tx to peer"); //, os.str());
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
        blob_t response_blob;
        auto r = w().txlog.cancel(track, response_blob);
        if (is_ko(r)) {
            return r;
        }
        return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_cancel, response_blob));
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
            blob_t response_blob;
            {
                auto r = w().txlog.register_transfer(blob, tid(), response_blob);
                if (is_ko(r)) {
                    return r;
                }
            }
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
            return w().txlog.register_invoice(blob);
        }
        break;
        case svc_cancel: {
            log("received svc_cancel");
            return w().txlog.cancel(blob);
        }
        break;
        case svc_tx: {
            log("received svc_tx");
            return w().txlog.register_tx(blob);
        }
        break;
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

