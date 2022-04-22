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
#include "account_t.h"
#include <us/gov/io/cfg.h>
#include <us/gov/socket/client.h>
#include <chrono>

#define loglevel "gov/peer"
#define logclass "account_t"
#include "logs.inc"

using c = us::gov::peer::account_t;
using namespace us::gov;
using namespace std::chrono;

size_t std::hash<c>::operator() (const c& g) const noexcept {
    auto h1 = std::hash<uint32_t>{}(g.net_address);
    auto h2 = std::hash<uint16_t>{}(g.port);
    return h1 ^ (h2 << 1);
}

c::account_t(host_t net_address, port_t port, uint16_t seen, uint16_t w): net_address(net_address), port(port), seen(seen), w(w) {
}

c::account_t(const account_t& other): net_address(other.net_address), port(other.port), seen(other.seen), w(other.w) {
}

c& c::operator = (int zero) {
    net_address = 0;
    port = 0;
    seen = 0;
    w = 1;
    return *this;
}

bool c::operator == (const account_t& other) const {
    return net_address == other.net_address && port == other.port;
}

c& c::operator = (const c& other) {
    net_address = other.net_address;
    port = other.port;
    seen = other.seen;
    w = other.w;
    return *this;
}

ko c::check(channel_t channel) const {
    if (unlikely(!socket::client::is_valid_ip(net_address, channel))) {
        return "KO 22154 Invalid net-address.";
    }
    if (unlikely(port == 0)) {
        auto r = "KO 22155 Invalid TCP port.";
        log(r);
        return r;
    }
    return ok;
}

string c::endpoint() const {
    ostringstream os;
    print_endpoint(os);
    return os.str();
}

void c::print_endpoint(ostream& os) const {
    os << socket::client::endpoint(net_address, port);
}

void c::dump(ostream& os) const {
    //auto n = duration_cast<hours>(chrono::system_clock::now().time_since_epoch()).count() / 24 - seen;
    os << socket::client::endpoint(net_address, port) << '\n';
}

void c::dump_as_seeds(ostream& os) const {
    os << socket::client::endpoint(net_address, port) << '\n';
}

void c::hash(crypto::ripemd160& h) const {
    h << net_address << port << seen << w;
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    h.write(net_address);
    h.write(port);
    h.write(seen);
    h.write(w);
}

void c::hash_data(hasher_t& h) const {
    h.write(net_address);
    h.write(port);
    h.write(seen);
    h.write(w);
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(net_address) +
        blob_writer_t::blob_size(port) +
        blob_writer_t::blob_size(seen) +
        blob_writer_t::blob_size(w);
 }

void c::to_blob(blob_writer_t& writer) const {
    writer.write(net_address);
    writer.write(port);
    writer.write(seen);
    writer.write(w);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = reader.read(net_address);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(port);
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(seen);
        if (is_ko(r)) return r;
    }
    if (reader.header.version == 7) {
        //v7 and 8 are written as v7, so we need to use heuristics
        if (reader.cur == reader.end) {
            w = 1;
            return ok;
        }
        {
            auto r = reader.read(w);
            if (is_ko(r)) return r;
        }
        if (w != 0) { //hoping no next field (net_address of next record, or end, or from unknown next object) is 1
            reader.cur-=2; //v7 legit
        }
        w = 1;
        return ok;
    }
    {
        auto r = reader.read(w);
        if (is_ko(r)) return r;
    }
    return ok;
}

