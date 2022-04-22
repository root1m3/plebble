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
#include "endpoint_t.h"

#include <sstream>
#include <chrono>

#include <us/gov/socket/client.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/socket/client.h>
#include <us/gov/likely.h>
#include <us/gov/stacktrace.h>

#define loglevel "wallet/trader"
#define logclass "endpoint"
#include <us/gov/logs.inc>

using namespace us::wallet;
using namespace us::wallet::trader;
using namespace std;
using namespace std::chrono;
using c = us::wallet::trader::endpoint_t;
using us::ko;

c::endpoint_t(): chan(0) {
    log("endpoint_t constructor A", chan);
}

c::endpoint_t(const string& ep): chan(0) {
    log("endpoint_t constructor B", ep, chan);
    from_string(ep);
}

c::endpoint_t(const hash_t& pkh, const string& subhome): pkh(pkh), wloc(subhome), chan(0) {
    log("endpoint_t constructor C", pkh, wloc, chan);
}

c::endpoint_t(const endpoint_t& other): pkh(other.pkh), wloc(other.wloc), chan(other.chan) {
    log("endpoint_t constructor D", pkh, wloc, chan);
}

c& c::operator = (const c& other) {
    pkh = other.pkh;
    wloc = other.wloc;
    chan = other.chan;
    log("endpoint_t operator = ", pkh, wloc, chan);
    return *this;
}

bool c::operator == (const c& other) const {
    return pkh == other.pkh && wloc == other.wloc && chan == other.chan;
}

bool c::operator != (const c& other) const {
    return pkh != other.pkh || wloc != other.wloc || chan != other.chan;
}

hostport_t c::decode_ip4() const {
    return decode_ip4(pkh);
}

string c::decode_ip4_string() const {
    return us::gov::socket::client::endpoint(decode_ip4());
}

string c::decode_ip4_string(const hash_t& h) {
    return us::gov::socket::client::endpoint(decode_ip4(h));
}

hostport_t c::decode_ip4(const hash_t& h) {
    if (*reinterpret_cast<const uint64_t*>(&h[0]) != 0) return make_pair(0, 0);
    if (*reinterpret_cast<const uint32_t*>(&h[8]) != 0) return make_pair(0, 0);
    if (*reinterpret_cast<const uint16_t*>(&h[12]) != 0) return make_pair(0, 0);
    return make_pair(*reinterpret_cast<const uint32_t*>(&h[16]) ,*reinterpret_cast<const uint16_t*>(&h[14]));
}

hash_t c::encode_ip4(const hostport_t& ipport) {
    hash_t h;
    *reinterpret_cast<uint64_t*>(&h[0]) = 0;
    *reinterpret_cast<uint32_t*>(&h[8]) =0;
    *reinterpret_cast<uint16_t*>(&h[12]) = 0;
    *reinterpret_cast<uint16_t*>(&h[14]) = ipport.second;
    *reinterpret_cast<uint32_t*>(&h[16]) = ipport.first;
    return h;
}

hash_t c::encode_ip4(host_t ip, port_t port) {
    hash_t h;
    *reinterpret_cast<uint64_t*>(&h[0]) = 0;
    *reinterpret_cast<uint32_t*>(&h[8]) = 0;
    *reinterpret_cast<uint16_t*>(&h[12]) = 0;
    *reinterpret_cast<uint16_t*>(&h[14]) = port;
    *reinterpret_cast<uint32_t*>(&h[16]) = ip;
    return h;
}

hash_t c::encode_ip4(const string& ipport) {
    pair<ko, pair<string, port_t>> r = us::gov::socket::client::parse_endpoint(ipport);
    if (r.first != ok) {
        return hash_t(0);
    }
    return encode_ip4(us::gov::socket::client::ip4_encode(r.second.first), r.second.second);
}

bool c::is_ip4() const {
    return decode_ip4().first != 0;
}

string c::filestr() const {
    ostringstream os;
    os << pkh;
    if (!wloc.empty()) {
        os << '.';
        os << wloc;
    }
    return os.str();
}

size_t c::blob_size() const {
    return blob_writer_t::blob_size(chan) + blob_writer_t::blob_size(pkh) + blob_writer_t::blob_size(wloc);
}

void c::to_blob(blob_writer_t& writer) const {
    log("writing endpoint_t");
    writer.write(chan);
    writer.write(pkh);
    writer.write(wloc);
}

ko c::from_blob(blob_reader_t& reader) {
    log("reading endpoint_t");
    {
        auto r = reader.read(chan);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(pkh);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(wloc);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

string c::to_string() const {
    ostringstream os;
    to_streamX(os);
    return os.str();
}

void c::to_streamX(ostream& os) const {
    if (chan != 0) os << chan << ' ';
    os << pkh;
    if (!wloc.empty()) {
        os << '.';
        os << wloc;
    }
    os << ' ';
}

ko c::from_string(const string& input) {
    istringstream is(input);
    return from_streamX(is);
}

ko c::from_streamX(istream& is) {
    string epf;
    is >> epf;
    if (is.fail()) {
        print_stacktrace(cout);
        auto r = "KO 66059";
        log(r);
        return r;
    }
    log("first word", epf);
    if (!epf.empty() && epf.size() < 6 && epf.find_first_not_of("0123456789") == std::string::npos) { //integer->channel
        log("numeric value => channel", epf);
        istringstream isc(epf);
        uint16_t ch;
        isc >> ch;
        if (isc.fail()) {
            auto r = "KO 66045 Invalid channel";
            log(r);
        }
        chan = ch;
        is >> epf;
    }
    log("next word", epf, "so far channel", chan);
    size_t n = count(epf.begin(), epf.end(), '.');
    log("num dots", n);
    if (n > 3) { //ip4+wloc
        log("ip4+wloc");
        size_t pos = 0;
        for (int i = 0; i < 4; ++i) pos = epf.find('.', pos) + 1;
        string spkh = string(epf, 0, pos - 1);
        if (epf.size() > pos - 1) {
            wloc = string(epf, pos, epf.size() - pos);
        }
        pkh = encode_ip4(spkh);
        log("pkh (enoded ip4)", pkh, "wloc", wloc);
    }
    else if(n == 3) { //ip4
        log("ip4");
        pkh = encode_ip4(epf);
        log("pkh (enoded ip4)", pkh, "wloc NO");
    }
    else if (n > 0) { //pkh+wloc
        log("pkh+wloc");
        auto pos = epf.find('.');
        string spkh = string(epf, 0, pos);
        if (epf.size() > pos) {
            wloc = string(epf, pos + 1, epf.size() - pos - 1);
        }
        istringstream isp(spkh);
        isp >> pkh;
        if (isp.fail()) {
            auto r = "KO 66059";
            log(r);
            return r;
        }
        log("pkh", pkh, "wloc", wloc);
    }
    else { //pkh
        istringstream isp(epf);
        isp >> pkh;
    }
    if (!is_ip4()) {
        if (pkh.is_zero()) {
            log("reset wloc on invalid ip4pkh");
            wloc.clear();
        }
    }
    logdump("ep> ", *this);
    return ok;
}

void c::clear() {
    pkh.zero();
    wloc.clear();
    chan = 0;
}

void c::hash(us::gov::crypto::ripemd160& h) const {
    h.write(chan);
    h.write(pkh);
    h.write(wloc);
}

void c::dump(const string& prefix, ostream& os) const {
    os << prefix << "chan " << chan << '\n';
    os << prefix << "pkh " << pkh << '\n';
    os << prefix << "wloc " << wloc << '\n';
}

