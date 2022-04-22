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

#include <us/gov/io/cfg0.h>
#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>

#include <us/wallet/protocol.h>
#include <us/wallet/wallet/local_api.h>

#include "business.h"

#define loglevel "trader/r2r/bid2ask/bid"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask;
using c = us::trader::r2r::bid2ask::bid::protocol;

const char *c::KO_30291 = "KO 30291 Invalid subcommand.";

const char* c::rolestr() const { return "bid"; }
const char* c::peer_rolestr() const { return "ask"; }
const char* c::get_name() const { return name; }

c::protocol(business_t& bz): b(bz) {
    basket.name = "customer";
}

void c::post_configure(ch_t& ch) {
    log("post_configure");
    b::post_configure(ch);
    assert(_workflow->cat != nullptr);
    assert(_workflow->inv != nullptr);
    assert(_workflow->pay != nullptr);
    assert(_workflow->rcpt != nullptr);
    _workflow->cat->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
    _workflow->inv->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
    _workflow->pay->set_mode(us::wallet::trader::workflow::item_t::mode_send, ch);
    _workflow->rcpt->set_mode(us::wallet::trader::workflow::item_t::mode_recv, ch);
}

string c::authorize_invoice(const string& txb58) const {
    if (autopay) {
        return "";
    }
    else {
        return "ASK human";
    }
}

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    static_assert((int)svc_begin >= (int)us::wallet::trader::trader_protocol::svc_end);
    if (svc < bid2ask::protocol::svc_begin) {
        return b::trading_msg(peer, svc, move(blob));
    }
    log("trading_msg", svc);
    assert(svc < bid2ask::protocol::svc_end);
    switch(svc) {
        case svc_select_product_ans: {
            log("shop_select_result");
            string msg;
            auto r = blob_reader_t::parse(blob, msg);
            if (is_ko(r)) {
                return r;
            }
            istringstream is(msg);
            hash_t prod;
            is >> prod;
            string code;
            is >> code;
            if (code == "A") {
                int am;
                is >> am;
                cash_t price;
                is >> price;
                cash_t reward;
                is >> reward;
                ch_t ch(_local_params, _local_params_mx);
                basket.addx(prod, am, price, reward, "");
                basket.incr_serial(basket, ch);
                log("updated basket", prod, am, price, reward);
                tder->update_peer(peer, move(ch)); //sh params are coming they'll refresh devices
                return ok;
            }
            if (code == "D") {
                string reason;
                getline(is, reason);
                log("denied product", code, reason);
                tder->push_OK(string("Peer couldn't add the requested product/s to the basket. Reason given: ") + reason);
                return ok;
            }
            return ok;
        }
    }
    return KO_29100;
}

void c::dump(ostream& os) const {
    os << "bid2ask bid";
}

void c::list_trades_bit(ostream& os) const {
    os << "bid2ask bid ";
}

c::chat_t::entry c::AI_chat(const chat_t&, peer_t&) {
    chat_t::entry ans;
    return ans;
}

bool c::requires_online(const string& cmd) const {
    if (b::requires_online(cmd)) return true;
    if (cmd == "select") return true;
    if (cmd == "inv_pay") return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    b::help_online(indent, os);
    twocol(indent, "select <product> <amount>", "Add/remove item to the basket (negative amount = remove)", os);
    twocol(indent, "inv_pay [message]", "Pay the invoice, optionally adding a message or reference", os);
}

void c::help_onoffline(const string& indent, ostream& os) const {
    b::help_onoffline(indent, os);
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    {
        auto r = b::exec_online(peer, cmd0, ch);
        if (r != WP_29101) {
            return move(r);
        }
    }
    log("exec_online", cmd0);
    string cmd;
    istringstream is(cmd0);
    is >> cmd;
    if (cmd == "select") {
        assert(_workflow->pay != nullptr);
        if (_workflow->pay->has_doc()) {
            auto r = "KO 40219 Payment has been already completed. The basket cannot be changed.";
            log(r);
            return move(r);
        }
        string product;
        is >> product;
        int amount = 0;
        is >> amount;
        if (is.fail()) {
            auto r = "KO 30291 Invalid input.";
            log(r);
            return move(r);
        }
        ostringstream msg;
        msg << product << ' ' << amount;
        auto blob = blob_writer_t::make_blob(msg.str());
        return peer.call_trading_msg(peer_t::trading_msg_in_t(tid(), svc_select_product, blob));
    }
    if (cmd == "inv_pay") {
        string msg;
        getline(is, msg);
        us::gov::io::cfg0::trim(msg);
        log("inv_pay", "msg", msg);
        if (_workflow->pay->has_doc()) {
            auto r = "KO 40219 Payment has been already completed.";
            log(r);
            return move(r);
        }
        if (!_workflow->cat->has_doc()) {
            auto r = "KO 40299 Catalogue not available.";
            log(r);
            return move(r);
        }
        if (!_workflow->inv->has_doc()) {
            auto r = "KO 40399 Invoice not available.";
            log(r);
            return move(r);
        }
        {
            ostringstream os;
            if (!_workflow->inv->verify(os)) {
                auto r = "KO 40339 Invoice is not valid.";
                log(r);
                return move(r);
            }
        }
        log("analysing invoice content");
        assert(_workflow->inv->doc != nullptr);
        assert(_workflow->cat->doc != nullptr);
        hash_t basket_hash=_workflow->inv->doc->params.get("basket_hash", hash_t(0));
        auto my_basket_hash = basket.hash_value();
        if (get<0>(my_basket_hash) != basket_hash) {
            auto r = "KO 40139 Invoice corresponds to a different basket content.";
            log(r);
            return move(r);
        }
        hash_t cat_pay_coin = _workflow->cat->doc->params.get("pay_coin", hash_t(0));
        hash_t cat_reward_coin = _workflow->cat->doc->params.get("reward_coin", hash_t(0));
        hash_t inv_pay_coin = _workflow->inv->doc->params.get("pay_coin", hash_t(0));
        hash_t inv_reward_coin = _workflow->inv->doc->params.get("reward_coin", hash_t(0));
        if (cat_pay_coin != inv_pay_coin) {
            auto r = "KO 40361 Invoice pay coin does not match the catalogue.";
            log(r);
            return move(r);
        }
        if (cat_reward_coin != inv_reward_coin) {
            auto r = "KO 40362 Invoice reward coin does not match the catalogue.";
            log(r);
            return move(r);
        }
        cash_t inv_pay_amount = _workflow->inv->doc->params.get("pay_amount", cash_t(0));
        cash_t inv_reward_amount = _workflow->inv->doc->params.get("reward_amount", cash_t(0));
        if (get<1>(my_basket_hash) != inv_pay_amount) {
            auto r = "KO 40342 Invoice pay amount does not match the basket.";
            log(r);
            return move(r);
        }
        if (get<2>(my_basket_hash) != inv_reward_amount) {
            auto r = "KO 40342 Invoice reward amount does not match the basket.";
            log(r);
            return move(r);
        }
        auto tx_encoded = _workflow->inv->doc->params.get("tx", "");
        if (tx_encoded.empty()) {
            auto r = "KO 40369 Invoice does not contain transaction.";
            log(r);
            return move(r);
        }

        tx_t tx;
        {
            auto r = tx.read(tx_encoded);
            if (is_ko(r)) {
                return r;
            }
        }
        {
            auto r = w().refresh_data();
            if (is_ko(r)) {
                return r;
            }
        }
        {
            auto r = w().invoice_verify(tx, inv_pay_amount, inv_pay_coin, inv_reward_amount, inv_reward_coin);
            if (is_ko(r)) {
                return r;
            }
        }
        {
            auto r = w().tx_pay(tx);
            if (is_ko(r)) {
                return r;
            }
        }
        payment_t* doc = create_payment(tx, msg);
        log("brand new payment created");
        {
            lock_guard<mutex> lock2(_workflow->mx);
            _workflow->pay->replace_doc(doc, ch);
            _workflow->pay->send_to(peer);
        }
        return ok;
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

c::payment_t* c::create_payment(const tx_t& tx, const string& msg) const {
    log("new payment");
    payment_t* doc;
    assert(tder != nullptr);
    payment_t::options o;
    {
        ostringstream os;
        tx.write_pretty(get_lang(), os);
        o.text = os.str();
    }
    o.kv.set("lang", "en");
    o.kv.set("trade_id", tder->id);
    o.kv.set("tx", tx.encode());
    hash_t basket_hash=_workflow->inv->doc->params.get("basket_hash", hash_t(0));
    hash_t pay_coin = _workflow->inv->doc->params.get("pay_coin", hash_t(0));
    hash_t reward_coin = _workflow->inv->doc->params.get("reward_coin", hash_t(0));
    cash_t pay_amount = _workflow->inv->doc->params.get("pay_amount", cash_t(0));
    cash_t reward_amount = _workflow->inv->doc->params.get("reward_amount", cash_t(0));
    o.kv.set("basket_hash", basket_hash);
    o.kv.set("pay_coin", pay_coin);
    o.kv.set("reward_coin", reward_coin);
    o.kv.set("pay_amount", pay_amount);
    o.kv.set("reward_amount", reward_amount);
    if (!msg.empty()) {
        o.kv.set("message", msg);
    }

    if (tder->peer_personality.id.is_not_zero()) {
        o.kv.set("refer_personality", tder->peer_personality.id);
    }
    o.sk = tder->my_personality.k_sign_docs();
    auto r = payment_t::create(o);
    if (is_ko(r.first)) {
        log(r.first);
        assert(r.second == nullptr);
        return nullptr;
    }
    return r.second;
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
            {0, "KO 30920"},
            {1, "This trade is finished."}, {2, "Archive this trade."},
            {3, "Invoice is ready to be paid."}, {4, "Pay the invoice."},
            {5, "Filling basket."}, {6, "Select products. Request invoice."},
            {7, "No catalogue."}, {8, "Request product catalogue."},

        }) {
        }
    };

    struct r_es_t: r_t {
        using b = r_t;

        r_es_t(): b({
            {0, "KO 30920"},
            {1, "Este trade ha terminado."}, {2, "Archivar este trade."},
            {3, "Factura preparada para pagar."}, {4, "Pague la factura."},
            {5, "Llenando cesta."}, {6, "Seleccione productos. Pida la factura."},
            {7, "Sin catalogo."}, {8, "Pida catalogo de productos."},

        }) {
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
    if (_workflow->pay->has_doc()) {
        return 1;
    }
    if (_workflow->inv->has_doc()) {
        return 3;
    }
    if (!_workflow->cat->has_doc()) {
        return 7;
    }
    return 5;
}

void c::judge(const string& lang) {
    auto t = i18n::r_t::resolver(lang);
    auto st = trade_state_();
    auto r = t.resolve(st);
    log("trade_state", st, r);
    _trade_state = make_pair(st, r);
    _user_hint = t.resolve(trade_state_() + 1);
}

