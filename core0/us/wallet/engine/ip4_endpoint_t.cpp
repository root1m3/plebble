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
#include "ip4_endpoint_t.h"

#include <sstream>
#include <chrono>

#include <us/gov/config.h>
#include <us/gov/socket/client.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#define loglevel "wallet/engine"
#define logclass "ip4_endpoint_t"
#include <us/gov/logs.inc>

using namespace us::wallet;
using namespace std;
using namespace std::chrono;
using c = us::wallet::engine::ip4_endpoint_t;
using us::ko;


c::ip4_endpoint_t(): shost("127.0.0.1"), port(CFG_WALLET_PORT), channel(CFG_CHANNEL) {
}

c::ip4_endpoint_t(shost_t host_, port_t port_, channel_t channel_): shost(host_), port(port_), channel(channel_) {
}

c::ip4_endpoint_t(host_t host_, port_t port_, channel_t channel_): shost(us::gov::socket::client::ip4_decode(host_)), port(port_), channel(channel_) {
}

c::ip4_endpoint_t(const ip4_endpoint_t& other): shost(other.shost), port(other.port), channel(other.channel) {
}

bool c::operator == (const ip4_endpoint_t& other) const {
    return shost == other.shost && port == other.port && channel == other.channel;
}

string c::to_string() const {
    ostringstream os;
    os << shost << ':' << port << " channel " << channel;
    return os.str();
}

us::shostport_t c::shostport() const {
    return shostport_t(shost, port);
}

size_t c::blob_size() const {
    int sz = blob_writer_t::blob_size(shost) +
             blob_writer_t::blob_size(port) +
             blob_writer_t::blob_size(channel);
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    writer.write(shost);
    writer.write(port);
    writer.write(channel);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        ko r = reader.read(shost);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(port);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(channel);
        if (is_ko(r)) return r;
    }
    return ok;
}

