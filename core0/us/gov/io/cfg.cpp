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
#include <us/gov/crypto/symmetric_encryption.h>

#include <string.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <cassert>

#include <us/gov/socket/client.h>
#include "cfg.h"
#include "types.h"

#define loglevel "gov/io"
#define logclass "cfg"
#include "logs.inc"

using namespace std;
using namespace us::gov::io;
using c = us::gov::io::cfg;

c::cfg(const keys_t::priv_t& privk, const string& home, vector<hostport_t>&& seed_nodes): b(privk, home), seed_nodes(seed_nodes) {
}

c::cfg(const cfg& other): b(other), seed_nodes(other.seed_nodes) {
}

c::~cfg() {
}

hostport_t c::parse_host(const string& addrport) {
    log("parse_host", addrport);
    string addr;
    port_t port;
    auto s = addrport.find(':');
    if (s == string::npos) {
        addr = addrport;
        port = CFG_PPORT;
    }
    else {
        addr = addrport.substr(0, s);
        ++s;
        if (s == addrport.size()) {
            port = CFG_PPORT;
        }
        else {
            string sp = addrport.substr(s);
            istringstream is(sp);
            is >> port;
            if (is.fail()) {
                cerr << "Invalid port in expression " << addrport << endl;
                port = CFG_PPORT;
            }
        }
    }
    log("addr", addr, "port", port);
    if (port == 0) {
        log("Invalid port in expression ", addrport);
        port = CFG_PPORT;
    }
    return make_pair(us::gov::socket::client::ip4_encode(addr), port);
}

vector<pair<c::pubkeyh_t, hostport_t>> c::parse_nodes_file2(const string& absfile, channel_t channel) {
    vector<pair<pubkeyh_t, hostport_t>> addrs;
    if (!file_exists(absfile)) {
        log("file does not exist", absfile);
        ofstream f(absfile);
        f << '\n';
    }
    ifstream f(absfile);
    while(f.good()) {
        string line;
        getline(f, line);
        if (line.empty()) continue;
        log("line", line);
        istringstream is(line);
        pubkeyh_t k;
        string addrport;
        is >> k;
        if (k.is_zero()) {
            log("Invalid publickeyhash", line, absfile);
            log("Invalid ip4 address ", addrport, "for channel", channel, "file", absfile);
        }
        is >> addrport;
        auto host = parse_host(addrport);
        if (!us::gov::socket::client::is_valid_ip(host.first, channel)) {
            host.first = 0;
        }
        if (host.first == 0) {
            log("Invalid ip4 address ", addrport, "for channel", channel, "file", absfile);
        }
        else {
            addrs.emplace_back(make_pair(k, move(host)));
        }
    }
    return move(addrs);
}

void c::parse_nodes_file(vector<hostport_t>& addrs, const string& absfile, channel_t channel) {
    if (!file_exists(absfile)) {
        log("file does not exist", absfile);
        ofstream f(absfile);
        f << '\n';
    }
    ifstream f(absfile);
    while(f.good()) {
        string addrport;
        getline(f, addrport);
        if (addrport.empty()) continue;
        auto host = parse_host(addrport);
        if (!us::gov::socket::client::is_valid_ip(host.first, channel)) {
            host.first = 0;
        }
        if (host.first == 0) {
            log("Invalid ip4 address", addrport, channel, absfile);
            cerr << "Invalid ip4 address " << addrport << " for channel " << channel << ", found in file " << absfile << endl;
        }
        else {
            addrs.emplace_back(move(host));
        }
    }
}

pair<us::ko, c*> c::load(channel_t channel, const string& home, bool gen) {
    auto x = b::load(home, gen);
    if (is_ko(x.first)) {
        assert(x.second == nullptr);
        return make_pair(x.first, nullptr);
    }
    string blocks_dir = abs_file(home, "blocks");
    if (!ensure_dir(blocks_dir)) {
        delete x.second;
        auto r = "KO 66843 Cannot create blocks dir.";
        log(r);
        return make_pair(r, nullptr);
    }
    string fs_dir = abs_file(home, "fsroot");
    if (!ensure_dir(fs_dir)) {
        delete x.second;
        auto r = "KO 68532 Cannot create fsroot dir.";
        log(r);
        return make_pair(r, nullptr);
    }
    string locking_dir = abs_file(home, "locking");
    if (!ensure_dir(locking_dir)) {
        delete x.second;
        auto r = "KO 65943 Cannot create locking-programs dir.";
        log(r);
        return make_pair(r, nullptr);
    }
    vector<hostport_t> addrs;
    parse_nodes_file(addrs, abs_file(home, "nodes.manual"), channel);
    parse_nodes_file(addrs, abs_file(home, "nodes.auto"), channel);
    parse_nodes_file(addrs, abs_file(home, "nodes.distr"), channel);
    auto ret = make_pair(nullptr, new cfg(x.second->keys.priv, x.second->home, move(addrs)));
    delete x.second;
    return ret;
}

