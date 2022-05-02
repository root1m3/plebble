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
#include "device_t.h"
#include <random>
#include <fstream>
#include <limits>
#include <us/gov/auth/peer_t.h>
#include <us/gov/io/cfg1.h>

#define loglevel "wallet/engine"
#define logclass "device_t"
#include <us/gov/logs.inc>

using namespace us::wallet::engine;
using c = us::wallet::engine::device_t;

string c::default_name("my_device");

void c::to_stream(ostream& os) const {
    if (pub.valid) {
        os << pub << ' ';
    }
    os << (subhome.empty() ? "-" : subhome) << ' ' << name << '\n';
}

pair<bool, device_t> c::from_stream(istream& is) { //v 2
    pair<bool, device_t> d;
    d.first = true;
    auto g = is.tellg();
    string w;
    is >> w;
    if (w == "pin") {
        pin_t pin;
        is >> pin;
        d.second.encode_pin(pin);
    }
    else {
        is.seekg(g);
        is >> d.second.pub;
    }
    is >> d.second.subhome;
    if (d.second.subhome == "-") d.second.subhome.clear();
    getline(is, d.second.name);
    io::cfg0::trim(d.second.name);
    if (is.fail()) {
        d.first = false;
    }
    return move(d);
}

pair<bool, device_t> c::from_stream_prev(istream& is) { // v 1
    log("from stream prev");
    return from_stream_v1(is);
}

pair<bool, device_t> c::from_stream_v1(istream& is) { // v 1
    log("from stream prev");
    pair<bool, device_t> d;
    d.first = true;
    is >> d.second.pub;
    is >> d.second.subhome;
    if (d.second.subhome == "-") d.second.subhome.clear();
    getline(is, d.second.name);
    io::cfg0::trim(d.second.name);
    if (is.fail()) {
        d.first = false;
    }
    return move(d);
}

pin_t c::decode_pin() const {
    if (pub.valid) return 0;
    return *reinterpret_cast<const pin_t*>(&pub.data[0]);
}

void c::encode_pin(uint16_t pin) {
    *reinterpret_cast<uint16_t*>(&pub.data[0]) = pin;
    pub.valid = false;
}

void c::dump(ostream& os) const {
    if (pub.valid) {
        os << "[pubkey " << pub << "] [home " << subhome << "] [name " << name << ']';
    }
    else {
        os << "[pubkey (waiting for device)] [home " << subhome << "] [name " << name << ']';
    }
}

