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
#include "okev.h"

#include <string>

#include <us/gov/config.h>
#include <us/gov/io/blob_reader_t.h>
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/ko.h>

#define loglevel "test/net"
#define logclass "okev"
#include <us/gov/logs.inc>

#include "../assert.inc"

using namespace us;
using namespace us::test;
using c = us::test::okev;

c::okev(): b(3, 0) {
}

c::~okev() {
}

string c::name() const {
    return "test::okev";
};

void c::hash_data_to_sign(us::gov::crypto::ec::sigmsg_hasher_t& h) const {
    b::hash_data_to_sign(h);
    h.write(src_port);
    h.write(payload);
}

void c::hash_data(hasher_t& h) const {
    b::hash_data(h);
    h.write(src_port);
    h.write(payload);
}

void c::write_pretty_en(ostream& os) const {
    os << "---transaction---------------" << endl;
    os << "  test transaction type " << eid << " (test payload)" << endl;
    os << "  timestamp: " << ts << endl;
    os << endl;
    os << "payload size: " << payload.size() << endl;
    os << "source port " << src_port << endl;
    os << "-/-transaction---------------" << endl;
}

void c::write_pretty_es(ostream& os) const {
    os << "---transaction---------------" << endl;
    os << "  test transaction type " << eid << " (test payload)" << endl;
    os << "  timestamp: " << ts << endl;
    os << endl;
    os << "payload size: " << payload.size() << endl;
    os << "source port " << src_port << endl;
    os << "-/-transaction---------------" << endl;
}

size_t c::blob_size() const {
    auto sz = b::blob_size() +
        blob_writer_t::blob_size(payload) +
        blob_writer_t::blob_size(src_port);
    //cout << "okev sz = " << sz << endl;
    return sz;
 }

void c::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    writer.write(payload);
    writer.write(src_port);
}

ko c::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        assert(is_ok(r));
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(payload);
        assert(is_ok(r));
        if (is_ko(r)) return r;
    }
    {
        auto r = reader.read(src_port);
        assert(is_ok(r));
        if (is_ko(r)) return r;
    }
    return ok;
}

/*
        ec::sigmsg_hasher_t::value_type get_hash() const {
            ec::sigmsg_hasher_t h;
            write_sigmsg(h);
            ec::sigmsg_hasher_t::value_type v;
            h.finalize(v);
            return move(v);
        }
*/
/*
        datagram* get_datagram(uint16_t seq) const {
            ostringstream os;
            to_stream(os);
            string payload=os.str();
            datagram* d = new us::gov::socket::datagram(us::gov::protocol::gov_engine_custom_ev, seq, payload);
            string payloadcheck;
            //cout << "check " << payload << endl;
            assert(d->parse(payloadcheck));
            assert(payload==payloadcheck);
            return d;
        }
*/

/*
void to_stream(ostream& os) const {
    b::to_stream(os);
    if (!payload.empty())
    os << us::gov::crypto::b58::encode(payload) << ' ';
    else
    os << "- ";

    os << src_port << ' ';
}

static okev* from_stream(istream& is) {
    log("okev::from_stream");
    //cout << "okev::from_stream" << endl;
    okev* t=new okev();
    b::from_stream(*t, is);
    if (is.fail()) {
        auto r="KO 76985";
        log(r);
        cout << r << endl;
        assert(false);
    }
    {
        string s;
        is >> s;
        if (s=="-") {
            t->payload.clear();
        }
        else {
            if (!us::gov::crypto::b58::decode(s, t->payload)) {
                auto r="KO 76986";
                log(r);
                cout << r << endl;
                assert(false);
                delete t;
                return nullptr;
            }
        }
    }
    is >> t->src_port;
    if (is.fail()) {
        auto r="KO 76987";
        log(r);
        cout << r << endl;
        assert(false);
        delete t;
        return nullptr;
    }
    return t;
}
*/


