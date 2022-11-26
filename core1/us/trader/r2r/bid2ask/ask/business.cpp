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
#include "business.h"
#include <fstream>

#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg0.h>

//#include <us/wallet/wallet/local_api.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/trader/r2r/bid2ask/types.h>
#include <us/trader/workflow/consumer/cat.h>

#define loglevel "trader/r2r/bid2ask/ask"
#define logclass "business_t"
#include <us/gov/logs.inc>

using namespace us::trader::r2r::bid2ask::ask;
using c = us::trader::r2r::bid2ask::ask::business_t;
using std::string;
using std::pair;
using us::ko;

c::business_t() {
    name = "seller";
}

ko c::init(const string& r2rhome, protocol_factories_t& f) {
    auto r = b::init(r2rhome, f);
    if (is_ko(r)) {
        return r;
    }
    log("init shop", home);
    assert(!home.empty());
    stock.name = "stock";
    auto e = load_coinsx();
    if (is_ko(e)) {
        return e;
    }
    fill_stock();
    log("shop initiated @", home);

    return ok;
}

c::protocol_factory_id_t c::protocol_factory_id() const {
    return protocol_factory_id_t(c::protocol::name, "ask");
}

void c::register_factories(protocol_factories_t& protocol_factories) {
    struct my_protocol_factory_t: protocol_factory_t {

        my_protocol_factory_t(c* bz): bz(bz) {}

        pair<ko, value_type*> create() override {
            auto a = new business_t::protocol(*bz);
            log("created protocol", protocol::name, "instance at", a);
            return make_pair(ok, a);
        }

        c* bz;
    };
    protocol_factories.register_factory(protocol_factory_id(), new my_protocol_factory_t(this));
    assert(protocol_factories.find(protocol_factory_id()) != protocol_factories.end());
}

string c::homedir() const { /// home is homedir() cached
    ostringstream os;
    os << r2rhome << '/' << protocol::name << "/ask";
    log("homedir", os.str());
    return os.str();
}

ko c::load_coinsx() {
    log("loading coins from data file", home + "/data");
    ifstream is(home + "/data");
    is >> address;
    if (is.fail()) {
        auto r = "KO 42931 cannot find data file or file is corrupt.";
        log(r, home + "/data");
        address = 0;
        recv_coin = 0;
        send_coin = 0;
        return r;
    }
    recv_coin = us::gov::io::shell_args::next_token(is);
    if (is.fail()) {
        auto r = "KO 44932 cannot find data file or file is corrupt.";
        log(r, home + "/data");
        address = 0;
        recv_coin = 0;
        send_coin = 0;
        return r;
    }
    send_coin = us::gov::io::shell_args::next_token(is);
    if ( is.fail() ) {
        auto r = "KO 449323 cannot find data file or file is corrupt.";
        log(r, home + "/data");
        address = 0;
        recv_coin = 0;
        send_coin = 0;
        return r;
    }
    log("address", address);
    log("receive coin", recv_coin);
    log("send coin", send_coin);
    return ok;
}

c::~business_t() {
    log("destroyed shop", home);
}

void c::fill_stock() {
    log("fill stock");
    stock.clear();
    ifstream is(home + "/vol");
    while( is.good() ) {
        string line;
        getline(is, line);
        if (line.empty()) continue;
        istringstream isl(line);
        hash_t prod;
        isl >> prod;
        int vol;
        isl >> vol;
        basket_t::item_t it;
        isl >> it.first;
        if (isl.fail()) {
            log("fill_stock FAILED charge price", "item", prod);
            continue;
        }
        it.second = 0;
        isl >> it.second;
        if (isl.fail()) {
            log("fill_stock FAILED reward", "item", prod);
            continue;
        }
        string desc;
        getline(isl, desc);
        log("add stock product", prod, vol, it.first, it.second, desc);
        stock.addx(prod, vol, it.first, it.second, desc);
    }
    assert(stock.serial == 0);
    ++stock.serial;
    log(stock.size(), "products in stock");
}

pair<ko, c::tx_t*> c::tx_charge_pay(us::wallet::wallet::local_api& w, cash_t recv_amount, cash_t send_amount) const {
    log("shop::tx_charge_pay, invoice: address", address, "recv", recv_amount, recv_coin, "send:", send_amount, send_coin);
    if (unlikely(recv_coin.is_zero())) {
        auto r = make_pair("KO 30028 Invalid recv coin.", nullptr);
        log(r.first);
        return r;
    }
    if (unlikely(address.is_zero())) {
        auto r = make_pair("KO 30029 Invalid address.", nullptr);
        log(r.first);
        return r;
    }
    if (send_coin != recv_coin) {
        if (send_amount > 0) {
            log("tx_charge_pay");
            return w.tx_charge_pay(address, recv_amount, recv_coin, send_amount, send_coin);
        }
    }
    return w.tx_charge_pay(address, recv_amount, recv_coin, 0, hash_t(0));
}

/*
std::pair<us::ko, us::wallet::trader::trader_protocol*> c::create_protocol(protocol_selection_t&& protocol_selection) {
    log("protocol from string", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != c::protocol::name) {
        auto r = "KO 20012 Invalid protocol";
        log(r, "protocol", protocol_selection.first, "not for me", c::protocol::name);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "ask") {
        auto r = "KO 60594 seller: I can only do ask.";
        log(r, protocol_selection.second);
        return make_pair(r, nullptr);
    }
    return create_protocol();
}
*/
/*
std::pair<us::ko, us::wallet::trader::trader_protocol*> c::create_opposite_protocol(protocol_selection_t&& protocol_selection) {
    log("create_opposite_protocol", protocol_selection.first, protocol_selection.second);
    if (protocol_selection.first != c::protocol::name) {
        auto r = "KO 20012 Invalid protocol";
        log(r, "protocol", protocol_selection.first, "not for me", c::protocol::name);
        return make_pair(r, nullptr);
    }
    if (protocol_selection.second != "bid") {
        auto r = "KO 60594 seller: I only match bid.";
        log(r, protocol_selection.second);
        return make_pair(r, nullptr);
    }
    return create_protocol();
}
*/

std::pair<us::ko, us::wallet::trader::trader_protocol*> c::create_protocol() {
    auto a = new business_t::protocol(*this);
    log("protocol instance at", a, 1);
    log("created protocol", protocol::name);
    return make_pair(ok, a);
}

void c::list_protocols(ostream& os) const {
    os << c::protocol::name << " ask\n";
}

void c::published_protocols(protocols_t& protocols, bool inverse) const {
    protocols.emplace_back(c::protocol::name, inverse ? "bid" : "ask"); //protocols I can talk to
}

c::catalogue_t* c::catalogue(const string& lang) {
    catalogue_t* doc{ nullptr };
    if (!stock.empty()) {
        catalogue_t::options o;
        o.xhours = 24000;
        o.kv.set("lang", lang);
        o.kv.set("pay_coin", recv_coin);
        o.kv.set("reward_coin", send_coin);
        o.kv.set("fields", us::trader::workflow::consumer::cat_t::fields());
        auto cat = stock.cat();
        o.kv.set("compact_form", cat.encode());
        {
            ostringstream os;
            cat.dump(os);
            o.text = os.str();
        }
        auto r = catalogue_t::create(o);
        if (is_ko(r.first)) {
            log(r.first);
            return nullptr;
        }
        return r.second;
    }
    return nullptr;
}

