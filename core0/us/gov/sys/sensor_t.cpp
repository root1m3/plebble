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
#include "sensor_t.h"

#define loglevel "gov/sys"
#define logclass "sensor_t"
#include <us/gov/logs.inc>

using c = us::gov::sys::sensor_t;
using namespace us::gov;
using namespace std;

c::sensor_t() {
}

void c::dump(const string& prefix, ostream& os) const {
/*
    os << "num_cores " << (int)num_cores << endl;
    os << "bogomips " << bogomips << endl;
    os << "load_avg_15 " << load_avg_15 << endl;
    os << "swap_free " << swap_free << endl;
    os << "ram_free " << ram_free << endl;
    os << "disk_free " << disk_free << endl;
    os << "govd_pcpu " << govd_pcpu << endl;
    os << "govd_pmem " << govd_pmem << endl;
    os << "walletd_pcpu " << walletd_pcpu << endl;
    os << "walletd_pmem " << walletd_pmem << endl;
    os << "max_edges " << max_edges << endl;
    os << "edges " << edges.size() << ' ';
    for (auto&i:edges) os << socket::client::ip4_decode(i.first) << ' ' << socket::client::ip4_decode(i.second) << ' ';
    os << "in_sync " << in_sync << ' ';
    os << "io" << endl;
    io.dump(os);
*/
}

void c::hash_data_to_sign(sigmsg_hasher_t& h) const {
    h.write((int)0);

/*
    h.write(num_cores);
    h.write(bogomips);
    h.write(load_avg_15);
    h.write(swap_free);
    h.write(ram_free);
    h.write(disk_free);
    h.write(govd_pcpu);
    h.write(govd_pmem);
    h.write(walletd_pcpu);
    h.write(walletd_pmem);
    h.write(max_edges);
    for (auto&i:edges) { h.write(i.first); h.write(i.second); }
    h.write(in_sync);
    io.hash_data_to_sign(h);
*/
}

void c::hash_data(hasher_t& h) const {
    h.write((int)0);
/*
    h.write(num_cores);
    h.write(bogomips);
    h.write(load_avg_15);
    h.write(swap_free);
    h.write(ram_free);
    h.write(disk_free);
    h.write(govd_pcpu);
    h.write(govd_pmem);
    h.write(walletd_pcpu);
    h.write(walletd_pmem);
    h.write(max_edges);
    for (auto&i:edges) { h.write(i.first); h.write(i.second); }
    h.write(in_sync);
    io.hash_data(h);
*/
}

size_t c::blob_size() const {
    return 0;
}

void c::to_blob(blob_writer_t& writer) const {
}

ko c::from_blob(blob_reader_t& reader) {
    return ok;
}

/*
void c::io_t::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    for (auto&i:*this) {
        h.write(i.first);
        i.second.hash_data_to_sign(h);
    }
}

void c::io_t::hash_data(crypto::ripemd160& h) const {
    for (auto&i:*this) {
        h.write(i.first);
        i.second.hash_data(h);
    }
}

void c::io_t::to_stream(ostream& os) const {
    os << size() << ' ';
    for (auto& i:*this) {
        os << i.first << ' ';
        i.second.to_stream(os);
    }
}

void c::io_t::from_stream(istream& is) { // v4
    size_t n;
    is >> n;
    if (is.fail()) return;
    clear();
    for (size_t i=0; i<n; ++i) {
        uint16_t svc;
        is >> svc;
        if (unlikely(is.fail())) {
            return;
        }
        auto p=emplace(svc,iodata()).first;
        p->second.from_stream(is);
    }
}


void c::io_t::dump(ostream& os) const {
    for (auto&i:*this) {
        os << i.first << ' ';
        i.second.dump(os);
        os << endl;
    }
}

void c::io_t::from_counters() {
    clear();
    for (auto& i:socket::datagram::counters) {
        emplace(i.first,iodata(*i.second));
    }
}

c::iodata::iodata(const socket::datagram::iodata& iod) {
    sent.num=iod.sent.num.load();
    sent.bytes=iod.sent.bytes.load();
    received.num=iod.received.num.load();
    received.bytes=iod.received.bytes.load();
}

void c::iodata::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    sent.hash_data_to_sign(h);
    received.hash_data_to_sign(h);
}

void c::iodata::hash_data(crypto::ripemd160& h) const {
    sent.hash_data(h);
    received.hash_data(h);
}

void c::iodata::to_stream(ostream& os) const {
    sent.to_stream(os);
    received.to_stream(os);
}

void c::iodata::from_stream(istream& is) { // v4
    sent.from_stream(is);
    received.from_stream(is);
}


void c::iodata::dump(ostream& os) const {
    os << "sent ";
    sent.dump(os);
    os << "received ";
    received.dump(os);
}

void c::iodata::data::hash_data_to_sign(crypto::ec::sigmsg_hasher_t& h) const {
    h.write(num);
    h.write(bytes);
}

void c::iodata::data::hash_data(crypto::ripemd160& h) const {
    h.write(num);
    h.write(bytes);
}

void c::iodata::data::to_stream(ostream& os) const {
    os << num << ' ' << bytes << ' ';
}

void c::iodata::data::from_stream(istream& is) { // v4
    is >> num;
    is >> bytes;
}

void c::iodata::data::dump(ostream& os) const {
    os << "num " << num <<  " bytes " << bytes << ' ';
}
*/

