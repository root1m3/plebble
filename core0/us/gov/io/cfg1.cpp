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
#include "cfg1.h"
#include <string.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <cassert>

#define loglevel "gov/io"
#define logclass "cfg1"
#include "logs.inc"

using namespace us::gov::io;
using c = us::gov::io::cfg1;

const char* c::KO_30291 = "KO 30291 invalid k file.";
const char* c::KO_97832 = "KO 97832 Secret key not found.";
const char* c::KO_50493 = "KO 50493 Cannot create home dir.";
const char* c::KO_60987 = "KO 60987 Invalid pubkey.";

c::cfg1(const keys_t::priv_t& privk, const string& home): keys(privk), b(home) {
    if (!keys.pub.valid) {
        cerr << KO_60987 << '\n';
        exit(1);
    }
}

c::cfg1(const cfg1& other): keys(other.keys), b(other) {
}

c::~cfg1() {
}

pair<ko, priv_t> c::load_sk(const string& home) {
    string keyfile = k_file(home);
    string pkb58;
    pair<ko, priv_t> r;
    {
        ifstream f(keyfile);
        if (!f.good()) {
            r.first = KO_30291;
            return move(r);
        }
        f >> r.second;
    }
    auto e = crypto::ec::keys::verifyx(r.second);
    if (is_ko(e)) {
        r.first = e;
        return move(r);
    }
    r.first = ok;
    return move(r);
}

string c::k_file(const string& home) {
    return abs_file(home, "k");
}

us::ko c::write_k(const string& home, const keys_t::priv_t& priv) {
    if (!ensure_dir(home)) {
        log(KO_50493, home);
        return KO_50493;
    }
    string file = k_file(home);
    {
        ofstream f(file);
        f << priv << '\n';
    }
    chmod(file.c_str(), S_IRUSR|S_IWUSR);
    return ok;
}

pair<us::ko, c*> c::load(const string& home, bool gen) {
    string keyfile = k_file(home);
    if (!file_exists(keyfile)) {
        if (!gen) {
            return make_pair(KO_97832, nullptr);
        }
        log("generating new keys at",home);
        crypto::ec::keys k = crypto::ec::keys::generate();
        ko r = write_k(home, k.priv);
        if (r != ok) {
            log(r);
            return make_pair(r, nullptr);
        }
    }
    auto x = b::load(home);
    if (x.first != ok) {
        log(x.first);
        return make_pair(x.first, nullptr);
    }
    auto pk = load_sk(home);
    if (pk.first != ok) {
        return make_pair(pk.first, nullptr);
    }
    return make_pair(ok, new c(pk.second, x.second.home));
}

