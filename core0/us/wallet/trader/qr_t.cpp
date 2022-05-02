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
#include "qr_t.h"
#include <sstream>
#include <chrono>
#include <us/gov/socket/client.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#define loglevel "wallet/trader"
#define logclass "qr_t"
#include <us/gov/logs.inc>

using c = us::wallet::trader::qr_t;
using namespace us::wallet;
using namespace us::wallet::trader;
using namespace std;
using namespace std::chrono;
using us::ko;

c::qr_t() {
}

c::qr_t(endpoint_t&& ep, protocol_selection_t&& p): endpoint(move(ep)), protocol_selection(move(p)) {
}

c::qr_t(const endpoint_t& ep, protocol_selection_t&& p): endpoint(ep), protocol_selection(move(p)) {
}

c::qr_t(const endpoint_t& ep): endpoint(ep) {
}

c::qr_t(const qr_t& other): endpoint(other.endpoint), protocol_selection(other.protocol_selection) {
}

c& c::operator = (const qr_t& other) {
    endpoint = other.endpoint;
    protocol_selection = other.protocol_selection;
    return *this;
}

size_t c::blob_size() const {
    return endpoint.blob_size() + protocol_selection.blob_size();
}

void c::to_blob(blob_writer_t& writer) const {
    log("writing qr_t");
    writer.write(endpoint);
    writer.write(protocol_selection);
}

ko c::from_blob(blob_reader_t& reader) {
    log("reading qr_t");
    {
        auto r = reader.read(endpoint);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(protocol_selection);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

ko c::from_streamX(istream& is) {
    {
        auto r = endpoint.from_streamX(is);
        if (is_ko(r)) {
            return r;
        }
    }
    return protocol_selection.from_streamX(is);
}

void c::to_streamX(ostream& os) const {
    endpoint.to_streamX(os);
    protocol_selection.to_streamX(os);
}

ko c::from(const string& s) {
    istringstream is(s);
    return from_streamX(is);
}

string c::to_string() const {
    ostringstream os;
    to_streamX(os);
    return os.str();
}

void c::dump(const string& pfx, ostream& os) const {
    os << pfx;
    endpoint.to_streamX(os);
    protocol_selection.to_streamX(os);
    os << '\n';
}

