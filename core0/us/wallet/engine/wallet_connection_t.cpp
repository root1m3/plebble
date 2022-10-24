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
#include "wallet_connection_t.h"

#include <sstream>
#include <chrono>

#include <us/gov/socket/client.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/likely.h>
#include <us/gov/stacktrace.h>

#define loglevel "wallet/engine"
#define logclass "wallet_connection_t"
#include <us/gov/logs.inc>

using namespace us::wallet;
using namespace std;
using namespace std::chrono;
using c = us::wallet::engine::wallet_connection_t;
using us::ko;

c::wallet_connection_t(): name_("default wallet") {
    log("constructor 1 ");
}

c::wallet_connection_t(const string& nm, const ip4_endpoint_t& ep): name_(nm), ip4_endpoint(ep) {
    log("constructor 3 ");
}

c::wallet_connection_t(uint64_t ts_, const string& addr_, const string& nm, const string& ssid_, const ip4_endpoint_t& ep): name_(nm), ssid(ssid_), addr(addr_), ip4_endpoint(ep), ts(ts_) {
    log("constructor 2 ");
    
}

c::wallet_connection_t(const wallet_connection_t& other): name_(other.name_), ssid(other.ssid), addr(other.addr), ip4_endpoint(other.ip4_endpoint), ts(other.ts) {
    log("copy constructor");
}

void c::dump(ostream& os) const {
    os << "name " << name_ << '\n';
    os << "ssid " << ssid << '\n';
    os << "addr " << addr << '\n';
    os << ip4_endpoint.to_string() << '\n';
}

c c::copy() const {
    log("copy");
    c wc(*this);
    wc.name_ = string("copy_") + name_;
    wc.ssid = "";
    wc.addr = "";
    wc.ts = 0;
    return wc;
}

string c::log_string() const {
    log("en-uk entry:");
    wallet_connection_t wc(name_, ip4_endpoint);
    auto r = wc.encode();
    log(string("str95=\"") + r + "\" #default wallet_connection blob b58");
    return r;
}

ko c::set_endpoint(const ip4_endpoint_t& ip4_endpoint_) {
    ip4_endpoint = ip4_endpoint_;
    return ok;
}

string c::get_title() const {
    ostringstream os;
    os << name_;
    log("get title for " + name_);
    if (ssid.empty()) {
        log("adding ssid", ssid);
        os << '_' << ssid;
    }
    if (!addr.empty()) {
        log("adding addr", addr);
        os << '_' << addr;
    }
    log("returning ", os.str());
    return os.str();
}

size_t c::blob_size() const {
    size_t sz = blob_writer_t::blob_size(name_) +
             blob_writer_t::blob_size(ssid) +
             blob_writer_t::blob_size(addr) +
             blob_writer_t::blob_size(ip4_endpoint) +
             blob_writer_t::blob_size(ts);
    log("blob_size", sz);
    return sz;
}

void c::to_blob(blob_writer_t& writer) const {
    log("to_blob", "cur", (uint64_t)(writer.cur - writer.blob.data()));
    writer.write(name_);
    writer.write(ssid);
    writer.write(addr);
    writer.write(ip4_endpoint);
    writer.write(ts);
}

ko c::from_blob(blob_reader_t& reader) {
    log("from_blob", "cur", (uint64_t)(reader.cur - reader.blob.data()));
    {
        ko r = reader.read(name_);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(ssid);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(addr);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(ip4_endpoint);
        if (is_ko(r)) return r;
    }
    {
        ko r = reader.read(ts);
        if (is_ko(r)) return r;
    }
    log("read wallet connection named", name_);
    return ok;
}

