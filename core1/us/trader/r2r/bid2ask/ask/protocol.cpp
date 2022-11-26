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
//#include <fstream>

#include <us/gov/io/cfg0.h>
#include <us/gov/io/shell_args.h>

#include <us/wallet/wallet/local_api.h>
#include <us/wallet/engine/daemon_t.h>

#include <us/trader/workflow/consumer/docs.h>
#include <us/trader/r2r/bid2ask/types.h>

#include "business.h"

#define loglevel "trader/r2r/bid2ask/ask"
#define logclass "protocol"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask::ask;
using c = us::trader::r2r::bid2ask::ask::protocol;
using std::string;
using us::ko;

const char* c::rolestr() const { return "ask"; }
const char* c::peer_rolestr() const { return "bid"; }
const char* c::get_name() const  { return name; }

c::protocol(business_t& bz): b(bz) {
    log("create bid2ask::ask protocol. home=", phome);
    basket.name = "customer_twin";
}

c::~protocol() {
    log("destroy protocol");
}

void c::sig_reload(ostream& os) {
    log("sig_reload");
}

void c::ensure_catalogue(ch_t& ch) {
    log("ensure_catalogue");
    assert(consumer_workflow != nullptr);
    assert(consumer_workflow->cat != nullptr);
    bool create{false};
    if (consumer_workflow->cat->has_doc()) {
        log("existing catalog");
        if (!consumer_workflow->cat->verify()) {
            log("existing catalogue doesn't verify");
            create = true;
        }
    }
    else {
        create = true;
        log("existing catalogue don't exist");
    }
    if (create) {
        log("replacing with new instance");
        auto cat = static_cast<business_t&>(business).catalogue("en");
        consumer_workflow->cat->replace_doc(cat, ch);
    }
}

bool c::on_signal(int sig, ostream& os) {
    b::on_signal(sig, os);
    log("on_signal", sig);
    switch (sig) {
    case 0: //RESET
        return sig_reset(os);
    case 1: //HARD RESET
        return sig_hard_reset(os);
    case 2: //RELOAD
        sig_reload(os);
        return false;
    }
    return false;
}

void c::init_workflows(ch_t& ch) {
    assert(consumer_workflow != nullptr);
    consumer_workflow->init_ask(ch);
}

ko c::on_attach(trader_t& tder_, ch_t& ch) {
    log("on_attach", "trace-w8i");
    auto r = b::on_attach(tder_, ch); //calls init_workflows
    if (is_ko(r)) {
        return r;
    }
    log("on_attach. returned", ch.to_string());
    ensure_catalogue(ch);
    ch.shared_params_changed |= ch.local_params->shared.set("coin_pay", static_cast<business_t&>(business).recv_coin);
    ch.shared_params_changed |= ch.local_params->shared.set("coin_reward", static_cast<business_t&>(business).send_coin);
    return move(r);
}

us::gov::io::blob_t c::push_payload(uint16_t pc) const {
    if (pc < push_begin) {
        return b::push_payload(pc);
    }
    assert(pc < push_end);
    blob_t blob;
    switch(pc) {
        case push_catalogue: {
            log("pushing catalogue");
            static_cast<business_t&>(business).stock.cat().write(blob);
        }
        break;
        case push_stock: {
            log("pushing stock");
            ostringstream os;
            static_cast<business_t&>(business).stock.write(blob);
        }
        break;
    }
    return move(blob);
}

string c::check_KYC() const { //yeah I know who you are, you are anonymous, come in!
    return "";
}

string c::check_KYC(const hash_t& product) const { //yeah I know who you are, you are anonymous, you're welcome!
    return "";
}

c::invoice_t* c::create_invoice(const string& lang, const basket_t& basket, const tx_t& tx) const {
    log("new invoice");
    invoice_t* doc;
    assert(tder != nullptr);
    invoice_t::options o;
    auto basket_info = basket.hash_value();
    {
        ostringstream os;
        tx.write_pretty(lang, os);
        o.text = os.str();
    }
    o.kv.set("lang", lang);
    log("WP 42873 1");
    o.kv.set("trade_id", tder->id);
    log("WP 42873 2");
    log("encoding tx");
    log("encoding tx", tx.encode());
    log("encoding tx");
    o.kv.set("tx", tx.encode());
    log("WP 42873 3");
    o.kv.set("basket_hash", get<0>(basket_info));
    log("WP 42873 4");
    o.kv.set("pay_coin", static_cast<business_t&>(business).recv_coin);
    log("WP 42873 5");
    o.kv.set("pay_amount", get<1>(basket_info));
    log("WP 42873 6");
    if (static_cast<business_t&>(business).send_coin.is_not_zero()) {
        o.kv.set("reward_coin", static_cast<business_t&>(business).send_coin);
        o.kv.set("reward_amount", get<2>(basket_info));
    }
    log("WP 42873 7");
    if (lang == "es") {
        o.kv.set("instrucciones", "Complete los campos input y output en la transaction 'tx'. Firme la transaccion resultante y enviela de vuelta.");
    }
    else {
        o.kv.set("instructions", "Complete the missing input and output fields in the transaction 'tx'. Sign the resulting transaction and send it back over.");
    }
    if (tder->peer_personality.id.is_not_zero()) {
        o.kv.set("refer_personality", tder->peer_personality.id);
    }
    o.sk = tder->my_personality.k_sign_docs();
    log("create invoice");
    auto r = invoice_t::create(o);
    if (is_ko(r.first)) {
        log(r.first);
        assert(r.second == nullptr);
        return nullptr;
    }
    return r.second;
}

c::receipt_t* c::create_receipt(const string& lang, const basket_t& basket, const tx_t& tx, const string& msg, const string& info) const {
    log("new receipt");
    receipt_t* doc;
    assert(tder != nullptr);
    receipt_t::options o;
    {
        ostringstream os;
        basket.dump("", "  * ", os);
        os << '\n';
        tx.write_pretty(lang, os);
        o.text = os.str();
    }
    auto basket_info = basket.hash_value();
    o.kv.set("lang", lang);
    o.kv.set("trade_id", tder->id);
    o.kv.set("tx", tx);
    o.kv.set("basket_hash", get<0>(basket_info));
    o.kv.set("pay_coin", static_cast<business_t&>(business).recv_coin);
    o.kv.set("pay_amount", get<1>(basket_info));
    if (static_cast<business_t&>(business).send_coin.is_not_zero()) {
        o.kv.set("reward_coin", static_cast<business_t&>(business).send_coin);
        o.kv.set("reward_amount", get<2>(basket_info));
    }
    if (!msg.empty()) {
        o.kv.set("message", msg);
    }
    if (!info.empty()) {
        o.kv.set("info", info);
    }
    o.kv.set("status", "PAID");
    o.kv.set("fields", basket_t::fields());
    if (tder->peer_personality.id.is_not_zero()) {
        o.kv.set("refer_personality", tder->peer_personality.id);
    }
    o.sk = tder->my_personality.k_sign_docs();
    auto r = receipt_t::create(o);
    if (is_ko(r.first)) {
        log(r.first);
        assert(r.second == nullptr);
        return nullptr;
    }
    return r.second;
}

ko c::select(peer_t& peer, const hash_t& product, int volume, bool superuser, ch_t& ch) {
    log("selecting product", product, volume, superuser);
    ostringstream msg;
    msg << product << ' ';
    string reason;
    if (enabled_select || superuser) {
        if (!consumer_workflow->pay->has_doc()) {
            reason = check_KYC(product);
        }
        else {
            reason = "Payment has already been completed.";
        }
    }
    else {
        reason = "select is disabled.";
    }
    if (reason.empty()) {
        assert(!ch.closed()); //is local_params ready for writing?
        auto auth = basket_t::transfer(static_cast<business_t&>(business).stock, basket, product, volume, basket, ch);
        msg << "A " << auth.first << ' ' << auth.second.first << ' ' << auth.second.second; //approved
        log("stock transfer returned: vol:", auth.first , "sale price per unit: ", auth.second.first, "reward price per unit:", auth.second.second, ch.to_string());
    }
    else {
        log("denied", reason);
        msg << "D " << reason; //denied
    }
    blob_t blob;
    blob_writer_t writer(blob, blob_writer_t::blob_size(msg.str()));
    writer.write(msg.str());
    logdump("basket> ", basket);
    return tder->call_trading_msg(peer, svc_select_product_ans, blob);
}

void c::dump(ostream& os) const {
    os << "bid2ask ask";
}

void c::list_trades_bit(ostream& os) const {
    os << "bid2ask ask ";
}

ko c::workflow_item_requested(workflow_item_t& i, peer_t& peer, ch_t& ch) {
    log("workflow_item_requested", i.name);
    if (i.name == workflow::consumer::catalogue_traits::name) {
        ensure_catalogue(ch);
        return ok;
    }
    if (i.name != us::trader::workflow::consumer::invoice_traits::name) {
        log("Not an invoice");
        return ok;
    }
    if (consumer_workflow->pay->has_doc()) {
        auto r = "KO 67599 Payment has already been completed.";
        log(r);
        return r;
    }
    string reason = check_KYC();
    if (!reason.empty()) {
        auto r = "KO 30297 KYC";
        log(r);
        return r;
    }
    auto v = basket.hash_value();
    if (get<1>(v) <= 0) {
        auto r = "KO 09488 Amount is zero.";
        log(r);
        return r;
    }
    {
        auto r = w().refresh_data();
        if (is_ko(r)) {
            return r;
        }
    }
    auto r = static_cast<business_t&>(business).tx_charge_pay(w(), get<1>(v), get<2>(v));
    if (is_ko(r.first)) {
        return r.first;
    }
    assert(consumer_workflow != nullptr);
    assert(consumer_workflow->inv != nullptr);
    invoice_t* doc = create_invoice(get_lang(), basket, *r.second);
    log("brand new invoice created");
    consumer_workflow->inv->replace_doc(doc, ch);
    log("workflow_item_requested returned ", ch.to_string());
    delete r.second;
    return ok;
}

ko c::on_receive(peer_t& peer, workflow_item_t& itm, workflow_doc0_t* doc0, ch_t& ch) {
    log("on_receive workflow_item", itm.name);
    if (&itm == consumer_workflow->pay) {
        if (consumer_workflow->pay->has_doc()) {
            delete doc0;
            auto r = "KO 67999 Payment has already been completed.";
            log(r);
            return r;
        }
        auto* doc = static_cast<us::trader::workflow::consumer::payment_t*>(doc0);
        log("received payment");
        if (doc->params.get("trade_id", hash_t(0)) != tder->id) {
            delete doc;
            auto r = "KO 68000 Payment doesn't correspond to this trade.";
            log(r);
            return r;
        }
        hash_t basket_hash = doc->params.get("basket_hash", hash_t(0));
        auto my_basket_hash = basket.hash_value();
        if (get<0>(my_basket_hash) != basket_hash) {
            delete doc;
            auto r = "KO 41139 Payment corresponds to a different basket content.";
            log(r);
            return r;
        }
        if (static_cast<business_t&>(business).recv_coin != doc->params.get("pay_coin", hash_t(0))) {
            delete doc;
            auto r = "KO 41100 Unexpected pay coin.";
            log(r);
            return r;
        }
        if (get<1>(my_basket_hash) != doc->params.get("pay_amount", cash_t(0))) {
            delete doc;
            auto r = "KO 41101 Unexpected pay amount.";
            log(r);
            return r;
        }
        if (static_cast<business_t&>(business).send_coin.is_not_zero()) {
            if (static_cast<business_t&>(business).send_coin != doc->params.get("reward_coin", hash_t(0))) {
                delete doc;
                auto r = "KO 41102 Unexpected reward coin.";
                log(r);
                return r;
            }
            if (get<2>(my_basket_hash) != doc->params.get("reward_amount", cash_t(0))) {
                delete doc;
                auto r = "KO 41103 Unexpected reward amount.";
                log(r);
                return r;
            }
        }
        auto tx_encoded = doc->params.get("tx", "");
        if (tx_encoded.empty()) {
            delete doc;
            auto r = "KO 41104 Payment does not contain transaction.";
            log(r);
            return r;
        }
        tx_t tx;
        {
            auto r = tx.read(tx_encoded);
            if (is_ko(r)) {
                delete doc;
                return r;
            }
        }
        log("signed tx", tx.encode());
        {
            ostringstream os;
            if (!doc->verify(os)) {
                delete doc;
                auto r = "KO 41105 Payment is not valid.";
                log(r);
                return r;
            }
        }
        {
            auto r = w().payment_verify(tx, get<1>(my_basket_hash), static_cast<business_t&>(business).recv_coin, get<2>(my_basket_hash), static_cast<business_t&>(business).send_coin);
            if (is_ko(r)) {
                delete doc;
                return r;
            }
        }
        string info;
        {
            blob_t blob;
            tx.write(blob);
            auto r = w().handle_tx_send(move(blob), info);
            if (is_ko(r)) {
                delete doc;
                return r;
            }
        }
        itm.replace_doc(doc, ch);

        receipt_t* rdoc = create_receipt(peer.get_lang(), basket, tx, consumer_workflow->pay->doc->params.get("message", ""), info);
        log("brand new receipt created, delivering it to peer");
        consumer_workflow->rcpt->replace_doc(rdoc, ch);
        consumer_workflow->rcpt->send_to(*tder, peer);
        {
            ostringstream os;
            os << "Thank you for shopping at " << business.name << '.';
            tder->send_msg(peer, os.str());
        }
    }
    else {
        b::on_receive(peer, itm, doc0, ch);
    }
    return ok;
}

ko c::trading_msg(peer_t& peer, svc_t svc, blob_t&& blob) {
    static_assert((int)svc_begin >= (int)trader_protocol::svc_end);
    if (svc < bid2ask::protocol::svc_begin) {
        return b::trading_msg(peer, svc, move(blob));
    }
    log("trading_msg", svc);
    assert(svc < bid2ask::protocol::svc_end);
    switch(svc) {
        case svc_relay_tx: {
            log("payment evidence");
            string info;
            return w().handle_tx_send(move(blob), info);

        }
        case svc_select_product: {
            log("select product");
            string msg;
            {
                auto r = blob_reader_t::parse(blob, msg);
                if (is_ko(r)) {
                    return r;
                }
            }
            log("select product", msg);
            hash_t product;
            int volume;
            istringstream is(msg);
            is >> product;
            is >> volume;
            if (is.fail()) {
                log("fail select");
                return ok;
            }
            ch_t ch(_local_params, _local_params_mx);
            {
                auto r = select(peer, product, volume, false, ch);
                if (is_ko(r)) {
                    return r;
                }
            }
            tder->update_peer(peer, move(ch));
            return ok;
        }
    }
    return KO_29100;
}

bool c::requires_online(const string& cmd) const {
    if (b::requires_online(cmd)) return true;
    if (cmd == "select") return true;
    return false;
}

void c::help_online(const string& indent, ostream& os) const {
    b::help_online(indent, os);
    twocol(indent, "select <product> <amount>", "Add item to buyer's basket (amount 0 remove)", os);
}

ko c::exec_online(peer_t& peer, const string& cmd0, ch_t& ch) {
    assert(!ch.closed());
    {
        auto r = b::exec_online(peer, cmd0, ch);
        if (r != WP_29101) {
            return move(r);
        }
    }
    string cmd;
    istringstream is(cmd0);
    is >> cmd;
    if (cmd == "select") {
        hash_t product;
        is >> product;
        int amount = 0;
        is >> amount;
        {
            auto r = select(peer, product, amount, true, ch);
            if (is_ko(r)) {
                return r;
            }
        }
        return ok;
    }
    auto r = WP_29101;
    log(r);
    return move(r);
}

c::chat_t::entry c::AI_chat(const chat_t& chat, peer_t& peer) {
    using namespace chrono;
    chat_t::entry response;
    if (chat.empty()) return move(response);
    if (chat.empty_peer_()) return move(response);
    if (chat.empty_me_()) {  //ans politely
        response.emplace_back("Hello from my shop.");
    }
    else if (chat.num_me_() == 1) { //if they insist on chatting tell we're closed.
        response.emplace_back("Chat is closed. I'll mute now.");
    }
    return move(response);
}

void c::judge(const string& lang) {
    b::judge(lang);
}

size_t c::blob_size() const {
    size_t sz = b::blob_size();
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
}

c::factory_id_t c::factory_id() const {
    return protocol_selection_t("bid2ask", "ask");
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

