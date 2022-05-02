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
#include "basket.h"
#include <us/wallet/protocol.h>
#include <us/wallet/wallet/local_api.h>
#include <us/wallet/trader/params_t.h>
#include <us/wallet/engine/daemon_t.h>
#include <us/trader/workflow/types.h>
#include <fstream>

#define loglevel "trader/workflow/consumer"
#define logclass "basket_t"
#include <us/gov/logs.inc>

using namespace us::trader::workflow;
using namespace us::trader::workflow::consumer;
using c = us::trader::workflow::consumer::basket_t;

c::basket_t() {
}

c::~basket_t() {
}

pair<vol_t, basket_item_t> c::transfer(basket_t& src, basket_t& dst, const hash_t& prod, vol_t vol0, const basket_t& basket_ch, ch_t& ch) { ///returns vol, price per unit, reward price per unit
    log("basket transfer");
    pair<vol_t, basket_item_t> ans;
    ans.first = vol0;
    if (vol0 == 0) return ans;
    if (vol0 < 0) {
        log("returning product");
        auto ans = transfer(dst, src, prod, -vol0, basket_ch, ch);
        ans.first = -ans.first;
        return move(ans);
    }
    assert(vol0 > 0);
    {
        lock_guard<mutex> lock(src.mx);
        auto p = src.find(prod);
        if (p == src.end()) {
            log("KO src basket doesn't contain product", prod);
            return ans;
        }
        ans.second = p->second.second;
    }
    unique_lock<mutex> lock1(src.mx, defer_lock);
    unique_lock<mutex> lock2(dst.mx, defer_lock);
    lock(lock1, lock2);
    auto& vol = ans.first;
    log("substracting stock", vol, prod);
    auto r = src.substract_(prod, vol, ans.second, ch);
    if (is_ko(r.first)) {
        return ans;
    }
    assert(r.second > 0);
    vol = r.second; //vol contains how many have been substracted from stock
    dst.add_(prod, vol, ans.second, ch);
    src.incr_serial(basket_ch, ch);
    dst.incr_serial(basket_ch, ch);
    return ans;
}

void c::set(ch_t& ch) {
    log("set basket serial in shared_params for basket ", name, ":", serial, "ch before", ch.to_string());
    ch.shared_params_changed = ch.local_params->shared.set("basket_serial", serial);
    log("ch after", ch.to_string());
}

void c::incr_serial(const basket_t& basket_ch, ch_t& ch) {
    ++serial;
    log("incr serial basket", name, &basket_ch, serial);
    if (ch.closed()) return;
    if (&basket_ch != this) return;
    log("setting shared_params");
    assert(name != "stock");
    set(ch);
}

pair<ko, vol_t> c::substract_(const prod_t& prod, vol_t vol, const basket_item_t& itm, ch_t& ch) {
    log("substract", prod, vol);
    auto i = find(prod);
    if (i == end()) {
        auto r = "KO 60594 product not found";
        log(r, prod);
        return make_pair(r, 0);
    }
    if (vol > i->second.first) {
        vol = i->second.first;
        auto r = "ST 60595 sold out";
        log(r, prod);
    }
    if (vol == 0) {
        auto r = "KO 60591 nothing to move. weird.";
        log("", prod);
        return make_pair(r, 0);
    }
    assert(i->second.first >= 0);
    i->second.first -= vol;
    if (i->second.first == 0) {
        log("just declared out of stock for product", prod);
        erase(i);
    }
    return make_pair(ok, vol);
}

void c::add_(const hash_t& prod, vol_t vol, const item_t& itm, ch_t& ch) {
    auto i = find(prod);
    if (i == end()) {
        log("adding new product vol:", vol, "product", prod);
        emplace(prod, make_pair(vol, itm));
    }
    else {
       log("adding + product. vol:", vol, "product:", prod);
       i->second.first += vol;
       assert(i->second.second == itm);
    }
}

void c::addx(const hash_t& prod, vol_t vol, const cash_t& price, const cash_t& reward, const string& itemdesc) {
    if (vol == 0) return;
    lock_guard<mutex> lock(mx);
    auto i = find(prod);
    if (i == end()) {
        if (vol <= 0) {
            return;
        }
        basket_item_t it;
        it.first = price;
        it.second = reward;
        emplace(prod, make_pair(vol, it));
        log("adding product from basket", prod, vol);
    }
    else {
        assert(i->second.second.first == price);
        assert(i->second.second.second == reward);
        i->second.first += vol;
        if (i->second.first <= 0) {
            log("deleting product from basket", prod);
            erase(i);
        }
    }
}

tuple<hash_t, cash_t, cash_t> c::hash_value() const {
    using sorted_t=map<hash_t, pair<vol_t, basket_item_t>>;
    sorted_t m;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: *this) {
            m.emplace(i);
        }
    }
    tuple<hash_t, cash_t, cash_t> ans;
    cash_t a = 0;
    cash_t b = 0;
    us::gov::crypto::ripemd160 hasher;
    for (auto& i: m) {
        hasher.write(i.first);
        hasher.write(i.second.first);
        hasher.write(i.second.second.first);
        hasher.write(i.second.second.second);
        a += i.second.first*i.second.second.first;
        b += i.second.first*i.second.second.second;
    }
    log("basket value", a, b);
    hasher.finalize(get<0>(ans));
    get<1>(ans) = a;
    get<2>(ans) = b;
    return move(ans);
}

consumer::cat_t c::cat() const {
    cat_t o;
    for (auto& i: *this) {
        o.emplace(i.first, i.second.second);
    }
    return move(o);
}

void c::dump(ostream& os) const {
    dump("", "  * ", os);
}

void c::dump(const string& pfx, ostream& os) const {
    dump(pfx, "  * ", os);
}

void c::dump(const string& pfx, const string& pfxitems, ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << pfx << "basket serial " << serial << ". " << size() << " items.\n";
    for (auto& i: *this) {
        os << pfx << pfxitems << i.first << ' ' << i.second.first << ' ' << i.second.second.first << ' ' << i.second.second.second << '\n';
    }
}

string c::fields() {
    return "product units price/unit reward/unit";
}

pair<cash_t, cash_t> c::value() const {
    lock_guard<mutex> lock(mx);
    pair<cash_t, cash_t> ans;
    ans.first = ans.second = 0;
    for (auto& i: *this) {
        ans.first += i.second.first * i.second.second.first;
        ans.second += i.second.first * i.second.second.second;
    }
    log("basket value", ans.first, ans.second);
    return ans;
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(name) + blob_writer_t::blob_size(serial) + b::blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    writer.write(name);
    writer.write(serial);
    b::to_blob(writer);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(name);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(serial);
        if (is_ko(r)) return r;
    }
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    return ok;
}

