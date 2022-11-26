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
#include "devices_t.h"

#include <random>
#include <fstream>
#include <limits>
#include <ctime>
#include <iostream>
#include <sys/stat.h>

#include <us/gov/auth/peer_t.h>
#include <us/gov/io/cfg1.h>

#define loglevel "wallet/engine"
#define logclass "devices_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::devices_t;

constexpr chrono::seconds c::attempts_t::forget;
char c::devices_t::d_version = '2';
ko c::KO_30291 = "KO 30291 Device not paired.";
ko c::KO_30292 = "KO 30292 conf not found.";

c::devices_t(const string& home): home(home) {
    log("constructor", home);
    assert(!home.empty());
    string filename = home + "/d";
    if (!us::gov::io::cfg0::file_exists(filename)) {
        save();
    }
    load();
    log("size", size());
    assert(!empty());
}

void c::load() {
    lock_guard<mutex> lock(mx);
    load_();
}

void c::save() const {
    lock_guard<mutex> lock(mx);
    save_();
}

ko c::load_() {
    string filename = home + "/d";
    clear();
    log("load authorized devices and config from", filename);
    ifstream is(filename);
    string ver;
    is >> ver;
    log("devices file version", ver);
    if (ver[0] != d_version) {
        auto r = "KO 89899 version mismatch.";
        log(r, d_version, ver[0]);
        return r;
    }
    is >> authorize_and_create_guest_wallet;
    log("authorize_and_create_guest_wallet", authorize_and_create_guest_wallet);
    is >> consume_pin;
    log("consume_pin", consume_pin);
    {
        string eol;
        getline(is, eol);
        log("eol", eol);
        assert(eol.empty());
    }
    while (is.good()) {
        auto d = device_t::from_streamX(is);
        if (is_ko(d.first)) {
            log("error loading device.", d.first);
            break;
        }
        if (!d.second.pub.valid) {
            prepaired.emplace(d.second.decode_pin(), d.second);
            log("added prepaired device. pin", d.second.decode_pin());
            continue;
        }
        log("loaded device pubkey:", d.second.pub, "name:", d.second.name, "subhome:", d.second.subhome);
        emplace(d.second.pub.hash(), d.second);
    }
    if (empty()) {
        log("white list is empty.", "adding console key");
        using io::cfg_id;
        auto f = cfg_id::load(home + "/rpc_client", true);
        if (is_ko(f.first)) {
            cerr << f.first << '\n';
            exit(1);
        }
        emplace(f.second->keys.pub.hash(), device_t(f.second->keys.pub, "", "console"));
        log("added", f.second->keys.pub);
        save_();
        delete f.second;
    }
    return ok;
}

void c::save_() const {
    string file = home + "/d";
    {
        ofstream os(file);
        os << d_version << '\n';
        os << (authorize_and_create_guest_wallet ? 1 : 0) << '\n';
        os << (consume_pin ? 1 : 0) << '\n';
        for (auto& i: *this) {
            i.second.to_stream(os);
        }
        for (auto& i: prepaired) {
            os << "pin " << i.first << ' ';
            i.second.to_stream(os);
        }
    }
    chmod(file.c_str(), S_IRUSR | S_IWUSR);
    log("saved devices file", file);
}

ko c::authorize(const pub_t& p, pin_t pin, request_data_t& request_data) {
    log("authorization request for device", p, "using pin", pin, "request_data", request_data);
    io::cfg0::trim(request_data);
    lock_guard<mutex> lock(mx);
    attempts.purge();
    auto i = find(p.hash());
    if (i != end()) {
        log("found in lookup table", p, "authorized!");
        attempts.purge(p);
        log("ignoring input subhome overriden by authtable", request_data, "->", i->second.subhome); 
/*
        if (!request_data.empty()) { // empty: ok if other subhome is returned; non-empty: ko if other subhome is returned.
            if (i->second.subhome != request_data) {
                auto r = "KO 78916 Intended subhome doesn't match the authorized subhome.";
                log(r);
                request_data = i->second.subhome; // return back the authorized subhome
                return r;
            }
        }
*/
//        if (i->second.subhome.empty()) {
//            return ok; //requested subhome
//        }
        if (!i->second.subhome.empty()) { //sysop can access any subhome 
            request_data = i->second.subhome; //inform assigned subhome for guests
        }
        return ok;
    }
    log("not found in pairing table", p);
    auto j = prepaired.find(pin);
    if (j != prepaired.end()) {
        log("found prepaired device");
        string subhome = j->second.subhome;
        if (subhome == "auto") { // Sysop has authorized to let user choose wallet ["new", empty, non-empty] if user use this pin.
            //if (request_data == "new") {
            ostringstream newuserdir;
            newuserdir << p.hash();
            subhome = newuserdir.str();
            log("new custodial wallet is requested. new=", request_data);
//            }
//            subhome = request_data; // User's choice
/*
            if (request_data.empty()) {
                ostringstream os;
                os << p.hash();
                subhome = os.str();
                log("auto subhome", subhome); //User chose root wallet; no way, he goes to a new custodial wallet
            }
            else {
                subhome = request_data; // User's choice
            }
*/
        }
        //using subhome given by sysop
/*
        if (!request_data.empty()) { // User didn't shut up.
            if (subhome != request_data) { // User wants to use certain wallet but other different was given.
                auto r = "KO 78916 Intended subhome doesn't match the authorized subhome.";
                log(r);
                return r;
            }
        }
*/
        request_data = subhome; //whatever user asks the right wallet is given.
        auto r = device_pair_(p, subhome, j->second.name, false);
        if (is_ko(r)) {
            return r;
        }
        log("paired device", p, subhome, j->second.name);
        if (consume_pin) {
            log("PIN consumed");
            prepaired.erase(j);
        }
        log("authorized for the first time.", "used pin", pin, "pubkey", p, "subhome", subhome);
        save_();
        attempts.purge(p);
        return ok;
    }
    log("Device not found in prepairing table.", p); //It's a new device
    if (!request_data.empty() && authorize_and_create_guest_wallet && request_data == "new") {
        ostringstream newuserdir;
        newuserdir << p.hash();
        request_data = newuserdir.str();
        log("new custodial wallet is requested. new=", request_data);
        auto r = device_pair_(p, request_data, "guest first seen device");
        if (is_ko(r)) {
            log(r, p, request_data);
            return r;
        }
        attempts.purge(p);
        return ok;
    }
    log("user used pin", pin);
    //if (pin != 0) {
        //TODO not-authorize if the throttle is < 10 seconds from the last trial
    //}
    attempts.add(p);
    log("not authorized. registered attempt.", p);
    return KO_30291;
}

pair<ko, pin_t> c::device_prepair_(pin_t pin, string subhome, string name) {
    log("device_prepair_ pin:", pin, "subhome:", subhome, "name:", name);
    io::cfg0::trim(subhome);
    io::cfg0::trim(name);
    auto r = valid_subhome(subhome);
    if (is_ko(r)) {
        return make_pair(r, 0);
    }
    if (name.empty()) name = device_t::default_name;
    if (pin == 0 && !allow_pin0) {
        log("KO 40390 Invalid pin");
        return make_pair("KO 40390 pin 0 is not allowed.", 0);
    }
    if (pin == numeric_limits<pin_t>::max() && random_pin) {
        log("generating random pin");
        mt19937_64 rng(random_device{}());
        log("generating random PIN in the range [", 1, "and", numeric_limits<pin_t>::max(), ")");
        uniform_int_distribution<pin_t> d(1, numeric_limits<pin_t>::max() - 1);
        uint16_t triesleft = numeric_limits<pin_t>::max();
        while(--triesleft > 0) {
            pin = d(rng);
            if (prepaired.find(pin) == prepaired.end()) {
                break;
            }
        }
        if (triesleft == 0) {
            log("KO 40393 Couldnt find unused pin");
            return make_pair("KO 40393 Couldnt find unused pin.", 0);
        }
    }
    if (prepaired.find(pin) != prepaired.end()) {
        log("KO 40396 PIN already exists.");
        return make_pair("KO 40396 PIN already exists.", 0);
    }
    log("size", prepaired.size(), "emplacing ", pin);
    prepaired.emplace(pin, device_t(subhome, name));
    log("size", prepaired.size());
    save_();
    return make_pair(ok, pin);
}

ko c::valid_subhome(string subhome) const {
    if (subhome == "-") subhome = "";
    if (subhome.empty()) {
        return ok;
    }
    if (subhome[0] == '/') {
        auto r = "KO 40392 Invalid subhome.";
        log(r);
        return r;
    }
    string subhome1 = io::cfg0::rewrite_path(subhome);
    if (subhome1 != subhome) {
        ko r = "KO 70182 subhome contains invalid characters";
        log(r, subhome, "!=", io::cfg0::rewrite_path(subhome));
        return r;
    }
    return ok;
}

ko c::device_pair_(const pub_t& pub, string subhome, string name, bool dosave) {
    log("device_pair_", pub, subhome, name, dosave);
    io::cfg0::trim(subhome);
    {
        auto r = valid_subhome(subhome);
        if (is_ko(r)) {
            return r;
        }
    }
    io::cfg0::trim(name);
    if (name.empty()) name = device_t::default_name;
    auto r = emplace(pub.hash(), device_t(pub, subhome, name));
    if (!r.second) {
        r.first->second.name = name; //rename - resubhome
        r.first->second.subhome = subhome; //rename - resubhome
    }
    if (dosave) save_();
    return ok;
}

pair<ko, string> c::device_unpair_(const pub_t& pub) {
    auto i = find(pub.hash());
    if (i == end()) {
        auto r = "KO 30293 Public key was not paired.";
        log(r);
        return make_pair(r, "");
    }
    pair<ko, string> r = make_pair(ok, i->second.subhome);
    erase(i);
    save_();
    return r;
}

ko c::device_unprepair_(pin_t pin) {
    auto i = prepaired.find(pin);
    if (i == prepaired.end()) {
        ko r = "KO 40399 Device not found.";
        log(r);
        return r;
    }
    prepaired.erase(i);
    save_();
    return ok;
}

pair<ko, pin_t> c::device_prepair(pin_t pin, string subhome, string name) {
    lock_guard<mutex> lock(mx);
    return device_prepair_(pin, subhome, name);
}

ko c::device_pair(const pub_t& pub, string subhome, string name) {
    lock_guard<mutex> lock(mx);
    return device_pair_(pub, subhome, name);
}

pair<ko, string> c::device_unpair(const pub_t& pub) {
    lock_guard<mutex> lock(mx);
    return device_unpair_(pub);
}

ko c::device_unprepair(pin_t pin) {
    lock_guard<mutex> lock(mx);
    return device_unprepair_(pin);
}

const device_t* c::get_device(const pub_t& pub) const {
    lock_guard<mutex> lock(mx);
    auto i = find(pub.hash());
    if (i == end()) return nullptr;
    return &i->second;
}

bool c::get_consume_pin() const {
    lock_guard<mutex> lock(mx);
    return consume_pin;
}

void c::set_consume_pin(bool b) {
    lock_guard<mutex> lock(mx);
    if (consume_pin != b) {
        consume_pin = b;
        save_();
    }
}

ko c::handle_conf(const string& key, const string& value, string& ans) {
    log("handle_conf", key, value);
    if (key == "authorize_and_create_guest_wallet") {
        lock_guard<mutex> lock(mx);
        if (value == "1") {
            if (!authorize_and_create_guest_wallet) {
                authorize_and_create_guest_wallet = true;
                save_();
            }
        }
        else if (value == "0") {
            if (authorize_and_create_guest_wallet) {
                authorize_and_create_guest_wallet = false;
                save_();
            }
        }
        else if (!value.empty()) {
            auto r = "KO 79138 Invalid value. It should be either 0, 1 or empty.";
            log(r);
            return r;
        }
        assert(value.empty());
        ostringstream os;
        os << "authorize_and_create_guest_wallet " << (authorize_and_create_guest_wallet ? 1 : 0) << '\n';
        ans = os.str();
        return ok;                
    }
    if (key == "consume_pin") {
        lock_guard<mutex> lock(mx);
        if (value == "1") {
            if (!consume_pin) {
                consume_pin = true;
                save_();
            }
        }
        else if (value == "0") {
            if (consume_pin) {
                consume_pin = false;
                save_();
            }
        }
        else if (!value.empty()) {
            auto r = "KO 79138 Invalid value. It should be either 0, 1 or empty.";
            log(r);
            return r;
        }
        assert(value.empty());
        ostringstream os;
        os << "consume_pin " << (consume_pin ? 1 : 0) << '\n';
        ans = os.str();
        return ok;                
    }
    if (key.empty()) {
        if (!value.empty()) {
            auto r = "KO 79137 non empty value for value.";
            log(r);
            return r;
        }
        ostringstream os;
        {
            lock_guard<mutex> lock(mx);
            os << "authorize_and_create_guest_wallet " << (authorize_and_create_guest_wallet ? 1 : 0) << '\n';
            os << "consume_pin " << (consume_pin ? 1 : 0) << '\n';
        }
        ans = os.str();
        return ok;
    }
    return KO_30292;
}

void c::dump(ostream& os) const {
    log("dump devices");
    {
        lock_guard<mutex> lock(mx);
        os << size() << " authorized devices:\n";
        os << "----------------------\n";

        for (auto& i: *this) {
            os << "  ";
            i.second.dump(os);
            os << '\n';
        }
        os << '\n';
        os << prepaired.size() << " pre-authorized devices:\n";
        os << "--------------------------\n";
        for (auto& i: prepaired) {
            os << "  pin " << i.first << ' ';
            i.second.dump(os);
            os << '\n';
        }
        os << '\n';
        os << "Pairing configuration:\n";
        os << "----------------------\n";
        os << "  Automatic authorization (w/o PIN) + custodial wallet: " << (authorize_and_create_guest_wallet ? "ON" : "OFF") << '\n';
        os << "  Consume PIN once used: " << (consume_pin ? "ON" : "OFF") << '\n';
    }
    os << '\n';
    attempts.dump(os);
}

void c::attempts_t::dump(ostream& os) const {
    os << "Recent unauthorized attempts:\n";
    os << "-----------------------------\n";
    lock_guard<mutex> lock(mx);
    if (empty()) {
        os << "  No unauthorized attempts registered in the past " << forget.count() << " seconds.\n";
        return;
    }
    for (auto& i: *this) {
        auto n = chrono::system_clock::to_time_t(i.second.first);
        os << "  device_pubkey " << i.first << ' ' << i.second.second << " attempts since " << ctime(&n);
    }
}

void c::attempts_t::dump(vector<string>& o) const {
    lock_guard<mutex> lock(mx);
    if (empty()) {
        return;
    }
    o.reserve(size());
    for (auto& i: *this) {
        ostringstream os;
        os << i.first << ' ' << i.second.second;
        o.push_back(os.str());
    }
}

void c::devices_t::attempts_t::add(const pub_t& p) {
    lock_guard<mutex> lock(mx);
    auto i = find(p);
    if (i != end()) {
        ++i->second.second;
        return;
    }
    emplace(p, make_pair(chrono::system_clock::now(), 1));
}

void c::attempts_t::purge() {
    auto n = chrono::system_clock::now();
    lock_guard<mutex> lock(mx);
    for (auto i = begin(); i != end();) {
        if (i->second.first - n < forget) {
            ++i;
            continue;
        }
        else {
            i = erase(i);
        }
    }
}

void c::attempts_t::purge(const pub_t& p) {
    lock_guard<mutex> lock(mx);
    auto i = find(p);
    if (i != end()) {
        log("removed", p, "from unauthorized attempts.");
        erase(i);
    }
}

