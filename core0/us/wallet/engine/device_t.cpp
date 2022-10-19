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

pair<ko, device_t> c::from_streamX(istream& is0) {
    log("from_stream", is0.tellg(), is0.good(), is0.fail(), is0.bad(), "st", is0.rdstate());
    assert(!is0.fail());
    pair<ko, device_t> d;
    d.first = ok;
    string line;
    getline(is0, line);
    if (is0.fail()) {
        d.first = "KO 86742 line read error in d file";
        log(d.first, "line", line, "g", is0.tellg(), "good", is0.good(), "fail", is0.fail(), "bad", is0.bad(), "rdstate", is0.rdstate());
        return d;
    }
    istringstream is(line);
    auto g = is.tellg();
    string w;
    is >> w;
    log("w", w);
    if (w == "pin") {
        pin_t pin;
        is >> pin;
        log("read pin", pin);
        d.second.encode_pin(pin);
    }
    else {
        is.seekg(g);
        is >> d.second.pub;
        log("read pubkey", d.second.pub);
    }
    if (is.fail()) {
        d.first = "KO 86743 read error in d file";
        log(d.first, "line", line);
        return d;
    }
    is >> d.second.subhome;
    if (d.second.subhome == "-") d.second.subhome.clear();
    getline(is, d.second.name);
    io::cfg0::trim(d.second.name);
    log("returning device subhome", d.second.subhome, "name", d.second.name);
    return d;
}

pin_t c::decode_pin() const {
    if (pub.valid) return 0;
    return *reinterpret_cast<const pin_t*>(&pub.data[0]);
}

void c::encode_pin(pin_t pin) {
    *reinterpret_cast<pin_t*>(&pub.data[0]) = pin;
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

