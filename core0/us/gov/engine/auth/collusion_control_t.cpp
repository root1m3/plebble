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
#include "collusion_control_t.h"
#include <us/gov/config.h>
#include <us/gov/socket/client.h>

#define loglevel "gov/engine/auth"
#define logclass "collusion_control_t"
#include <us/gov/engine/logs.inc>

using namespace us::gov::engine;
using c = us::gov::engine::auth::collusion_control_t;

uint8_t c::max_nodes_per_ip{CFG_MAX_NODES_PER_IP};

void c::dump(ostream& os) const {
    os << "ip4 [C|D|S] #nodes\n";
    for (auto& i: *this) {
        char cds = 'D'; //port closed, dynamic ip, static ip
        os << socket::client::ip4_decode(i.first) << " [" << cds << "] " << +i.second << '\n';
    }
    os << size() << " unique transport addresses.\n";
}

bool c::allow(host_t ip_addr) {
    log("collusion control", "allow?", socket::client::ip4_decode(ip_addr));
    auto i = find(ip_addr);
    if (i == end()) {
        emplace(ip_addr, 1);
        log("yes. first appearance");
        return true;
    }
    if (i->second >= max_nodes_per_ip) {
        log("Too many nodes with same ip address. rejecting", socket::client::ip4_decode(ip_addr), (int)i->second, '/', (int)max_nodes_per_ip);
        return false;
    }
    ++i->second;
    log((int)i->second, "nodes with same ip address", socket::client::ip4_decode(ip_addr), "max", (int)max_nodes_per_ip);
    return true;
}

void c::rm(host_t ip_addr) {
    auto i = find(ip_addr);
    if (i == end()) {
        return;
    }
    if (--i->second == 0) {
        erase(i);
    }
}

void c::update_(const nodes_t& n) {
    for (auto& i: n) {
        allow(i.second.net_address);
    }
}

