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
/* @file */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>

#include <us/gov/likely.h>
#include <us/gov/stacktrace.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/blob_writer_t.h>
#include <us/gov/socket/protocol.h>
#include <us/gov/id/protocol.h>
#include <us/gov/auth/protocol.h>
#include <us/gov/peer/protocol.h>
#include <us/gov/relay/protocol.h>
#include <us/gov/dfs/protocol.h>
#include <us/gov/engine/protocol.h>
#include <us/wallet/engine/protocol.h>

#include "datagram.h"
#include "busyled_t.h"

#define loglevel "gov/socket"
#define logclass "datagram"
#include "logs.inc"

using namespace us::gov::socket;
using c = us::gov::socket::datagram;

constexpr size_t c::h;
constexpr size_t c::maxsize;
constexpr size_t c::offset_channel;
constexpr size_t c::offset_size;
constexpr size_t c::offset_service;
constexpr size_t c::size_channel;
constexpr size_t c::size_size;
constexpr size_t c::size_service;

const char* c::KO_0001 = "KO 0001 channel mismatch.";
const char* c::KO_0002 = "KO 0002 too big.";
const char* c::KO_0003 = "KO 0003 too small.";
const char* c::KO_4038 = "KO 5933 invalid size";
const char* c::KO_4921 = "KO 4921 ::recv<0";
const char* c::KO_9021 = "KO 9021 ::recv 0. Peer disconnected.";
const char* c::KO_20292 = "KO 20293 Size would buffer overflow.";
const char* c::KO_20293 = "KO 20293 Invalid size.";

#if CFG_COUNTERS == 1
    c::counters_t c::counters;
#endif

#ifdef DEBUG
//#define FIND_DGRAM_LEAKS
#endif

#ifdef FIND_DGRAM_LEAKS
    #include <unordered_set>
    struct z:unordered_set<c*> {
    ~z() {
        cout << size() << " leaks:\n";
        for (auto& i: *this)
            i->dump(cout);
    }
    void emplace(c*d) {
//        cout << "CREATED " << d << '\n';
//        d->dump(cout);
        unordered_set<c*>::emplace(d);
    }
    void erase(iterator i) {
//        cout << "DESTROYED " << *i << '\n';
//        (*i)->dump(cout);
        unordered_set<c*>::erase(i);
    }
    };
    z x;
#endif

#ifdef FIND_DGRAM_DOUBLE_DELETE
    atomic<uint64_t> c::_next_serial{0};

    struct dd_t:unordered_set<uint64_t> {
        using b = unordered_set<uint64_t>;
        ~dd_t() {
            assert(empty());
        }
        void emplace(uint64_t s) {
            lock_guard<mutex> lock(mx);
            b::emplace(s);
        }
        void erase(iterator i) {
            b::erase(i);
        }
        mutex mx;
    };
    dd_t dd;
#endif

c::datagram(channel_t channel, uint16_t mode)  {
    log("constructor");
    switch(mode) {
        case 0: {  //used in encryption
            resize(h);
            encode_channel(channel);
            //encode_size(sz);
            encode_service(0);
            encode_sequence(0);
        }
        break;
        case 1: { //used in decryption
        }
        break;
        case 2: { //used in recv
            dend = 0;
        }
        break;
    }
    #ifdef FIND_DGRAM_LEAKS
        x.emplace(this);
    #endif
    #ifdef FIND_DGRAM_DOUBLE_DELETE
        _serial = _next_serial++;
        dd.emplace(_serial);
    #endif
}

c::datagram(channel_t channel, svc_t service, seq_t sequence, uint32_t payload_sz): service(service) {
    log("constructor 2");
    resize(h + payload_sz);
    encode_channel(channel);
    encode_size(h + payload_sz);
    encode_service(service);
    encode_sequence(sequence);
    dend = h + payload_sz;
    log("new datagram", this, "svc", service, "seq", sequence, "sz", size(), "payload sz", payload_sz, "ch", channel);
    #ifdef FIND_DGRAM_LEAKS
        x.emplace(this);
    #endif
    #ifdef FIND_DGRAM_DOUBLE_DELETE
        _serial = _next_serial.load();
        ++_next_serial;
        dd.emplace(_serial);
    #endif
}

c::datagram(const c& other): service(other.service), dend(other.dend) {
    log("constructor 3");
    resize(other.size());
    memcpy(data(), other.data(), other.size());
    #ifdef FIND_DGRAM_LEAKS
        x.emplace(this);
    #endif
    #ifdef FIND_DGRAM_DOUBLE_DELETE
        _serial = _next_serial.load();
        ++_next_serial;
        dd.emplace(_serial);
    #endif
}

c::~datagram() {
    log("destructor");
    #ifdef FIND_DGRAM_LEAKS
        {
        auto y = x.find(this);
        if (y == x.end()) {
            log("Already deleted", this);
            print_stacktrace(cerr);
            cerr << "Already deleted dgram" << this << '\n';
            abort();
        }
        x.erase(y);
        print_stacktrace(cerr);
        cerr << "first deleted " << this << '\n';
        log("first deleted", this);
        }
    #endif
    #ifdef FIND_DGRAM_DOUBLE_DELETE
        {
        lock_guard<mutex> lock(dd.mx);
        auto y = dd.find(_serial);
        if (y == dd.end()) {
            log("Already deleted", this, "serial", _serial);
            print_stacktrace(cerr);
            cerr << "Already deleted dgram" << this << " serial " << _serial << '\n';
            abort();
        }
        dd.erase(y);
        print_stacktrace(cerr);
        cerr << "first deleted " << this << " serial " << _serial << " dgrams alive " << dd.size() << '\n';
        log("first deleted", this, "serial", _serial, "dgrams alive", dd.size());
        }
    #endif
    #ifdef DGRAM_CATCH_DELETE
        {
        lock_guard<mutex> lock(_abort_if_delete_mx);
        auto i = _abort_if_delete.find(this);
        if (i != _abort_if_delete.end()) {
            print_stacktrace(cerr);
            cerr << "KO 66046" << endl;
            log("KO 66046");
            abort();
        }
        }
    #endif
    log("deleted datagram", this);
}

std::pair<ko, c*> c::encrypt(crypto::symmetric_encryption& se) const {
    log("encrypt dgram svc=", decode_service());
    auto d = new c(decode_channel(), 0);
    ko r = se.encrypt(*this, *d, h);
    if (unlikely(is_ko(r))) {
        delete d;
        return make_pair(r, nullptr);
    }
    d->dend = d->size();
    d->encode_size(d->dend);
    return make_pair(ok, d);
}

std::pair<ko, c*> c::decrypt(channel_t channel, crypto::symmetric_encryption& se) const {
    if (unlikely(service != 0)) {
        auto r = "KO 81920 svc should be 0 for encrypted dgram.";
        log("decrypt dgram", r, service);
        return make_pair(r, nullptr);
    }
    log("decrypt dgram", size());
    if (unlikely(size() < h)) {
        auto r = "KO 58948 Invalid dgram.";
        log("decrypt dgram", r, service);
        return make_pair(r, nullptr);
    }
    auto sz = decode_size();
    if (unlikely(sz != size())) {
        return make_pair(KO_20293, nullptr);
    }
    c* d = new c(channel, 1);
    ko r = se.decrypt(data() + h, sz - h, *d);
    if (unlikely(is_ko(r))) {
        delete d;
        return make_pair(r, nullptr);
    }
    d->dend = d->size();
    if (unlikely(d->size() < h)) {
        delete d;
        auto r = "KO 25994 Invalid decrypted size.";
        log(r);
        return make_pair(r, nullptr);
    }
    channel_t ch = d->decode_channel();
    if (unlikely(ch != channel)) {
        if (channel != all_channels) { //accepting any channel
            log(KO_0001, "comm", "read", dend, "evil", "wrong_channel", ch, "recv_channel", channel);
            #if CFG_COUNTERS == 1
                ++counters.wrong_channel;
            #endif
            delete d;
            return make_pair(KO_0001, nullptr);
        }
        log("enc dgram arrived targeting ALL channels.");
    }
    else {
        log("enc dgram arrived targeting my channel.", ch);
    }
    uint32_t decl_sz = d->decode_size();
    if (unlikely(decl_sz != d->size())) {
        log("KO 4038.1 - size", decl_sz, d->size());
        delete d;
        return make_pair(KO_4038, nullptr);
    }
    d->service = d->decode_service();
    return make_pair(ok, d);
}

bool c::validate_payload(const vector<uint8_t>& payload) {
    return h + io::blob_writer_t::blob_size(payload) < maxsize;
}

#ifdef DGRAM_CATCH_DELETE
    namespace {
        mutex _abort_if_delete_mx;
        unordered_map<const c*, int> _abort_if_delete;
    }

    void c::_begin_protect_delete(const datagram* d) {
        lock_guard<mutex> lock(_abort_if_delete_mx);
        auto i = _abort_if_delete.find(d);
        if (i == _abort_if_delete.end()) {
            _abort_if_delete.emplace(d, 1);
        }
        else {
            i->second++;
        }
    }

    void c::_end_protect_delete(const datagram* d) {
        lock_guard<mutex> lock(_abort_if_delete_mx);
        auto i = _abort_if_delete.find(d);
        if (i == _abort_if_delete.end()) {
            cerr << "KO 66045" << endl;
            log("KO 66045");
            abort();
        }
        if (i->second > 1) {
            --i->second;
        }
        else {
            _abort_if_delete.erase(i);
        }
    }
#endif

void c::reset_service(svc_t svc) {
    log("reset_encoded_service", service, "->", svc);
    encode_service(svc);
}

void c::encode_channel(channel_t channel) {
    assert(size() >= offset_channel + sizeof(channel_t));
    memcpy(data(), reinterpret_cast<uint8_t*>(&channel), sizeof(channel_t));
}

channel_t c::decode_channel() const {
    assert(size() >= offset_channel + sizeof(channel_t));
    channel_t chan;
    memcpy(reinterpret_cast<uint8_t*>(&chan), data() + offset_channel, sizeof(channel_t));
    return chan;
}

void c::encode_size(uint32_t sz) {
    assert(size() > offset_size + 3);
    (*this)[offset_size] = sz & 0xff;
    (*this)[offset_size + 1] = sz >> 8 & 0xff;
    (*this)[offset_size + 2] = sz >> 16 & 0xff;
    (*this)[offset_size + 3] = sz >> 24 & 0xff;
}

uint32_t c::decode_size() const {
    assert(size() >= offset_size + sizeof(uint32_t));
    uint32_t sz = (*this)[offset_size];
    static_assert(sizeof(uint32_t) == 4);
    sz |= (*this)[offset_size + 1] << 8;
    sz |= (*this)[offset_size + 2] << 16;
    sz |= (*this)[offset_size + 3] << 24;
    return sz;
}

void c::encode_service(svc_t svc) {
    assert(size() >= offset_service + sizeof(svc_t));
    static_assert(sizeof(svc_t) == 2);
    (*this)[offset_service] = svc & 0xff;
    (*this)[offset_service + 1] = svc >> 8 & 0xff;
}

svc_t c::decode_service() const {
    assert(size() >= offset_service + sizeof(svc_t));
    static_assert(sizeof(svc_t) == 2);
    svc_t svc = (*this)[offset_service];
    svc |= (*this)[offset_service + 1] << 8;
    return svc;
}

void c::encode_sequence(seq_t seq) {
    assert(size() >= offset_sequence + sizeof(seq_t));
    static_assert(sizeof(seq_t) == 2);
    (*this)[offset_sequence] = seq & 0xff;
    (*this)[offset_sequence+1] = seq >> 8 & 0xff;
}

seq_t c::decode_sequence() const {
    assert(size() >= offset_sequence + sizeof(seq_t));
    static_assert(sizeof(seq_t) == 2);
    seq_t seq = (*this)[offset_sequence];
    seq |= (*this)[offset_sequence + 1] << 8;
    return seq;
}

bool c::completed() const {
    log("completed? dend=", dend, "size=", size(), "completed=", (dend == size() && !empty()));
    return dend == size() && !empty();
}

ko c::recvfrom(int sock, channel_t channel, busyled_t& busyled) {
    log("recvfrom ch ", channel);
    if (unlikely(sock == -1)) {
        #if CFG_COUNTERS == 1
            ++counters.closed_connection;
        #endif
        auto r = "KO 7014 Connection is closed.";
        log(r);
        return r;
    }
    assert(EAGAIN == EWOULDBLOCK);
    if (dend < h) {
        if (size() < h) resize(h);
        int nread;
        while(true) {
            busyled.reset();
            nread = ::recv(sock, data() + dend, h - dend, 0);
            busyled.set();
            log("comm", "read", nread);
            if (unlikely(nread <= 0)) {
                if (errno == EAGAIN || errno == EINPROGRESS) {
                    log("recv 0 bytes.", "dend", dend, "errno", errno, strerror(errno), "preserving connection");
                    return ok;
                }
                if (nread == 0) {
                    if (errno == 0) {
                        log("recv 0 bytes.", "dend", dend, "errno", errno, strerror(errno), "KO_9021.1", "fd", sock);
                        return KO_9021;
                    }
                }
                log("comm", "read", nread, "unavailable", "KO 4921.1 errno", errno, "fd", sock);
                return KO_4921;
            }
            break;
        }
        #if CFG_COUNTERS == 1
            counters.bytes_received += nread;
        #endif
        dend += nread;
        if (dend < h) {
            #if CFG_COUNTERS == 1
                ++counters.infraheader_datagram;
            #endif
            log("comm", "read", dend, "infraheader_datagram");
            return ok;
        }
        auto ch = decode_channel();
        if (unlikely(ch != channel)) {
            if (channel != all_channels) { //accepting any channel
                log(KO_0001, "comm", "read", dend, "evil", "wrong_channel", +ch, "recv_channel", channel);
                #if CFG_COUNTERS == 1
                    ++counters.wrong_channel;
                #endif
                return KO_0001;
            }
            log("enc dgram arrived targeting ALL channels.");
        }
        else {
            log("enc dgram arrived targeting my channel.", ch);
        }
        auto sz = decode_size();
        if (unlikely(sz > maxsize)) {
            #if CFG_COUNTERS == 1
                ++counters.wrong_size;
            #endif
            log(KO_0002, "comm", "read", dend, "evil", "too_big", sz, maxsize);
            return KO_0002;
        }
        if (unlikely(sz < h)) {
            #if CFG_COUNTERS == 1
                ++counters.wrong_size;
            #endif
            log(KO_0003, "comm", "read", dend, "evil", "too_small", sz, h);
            return KO_0003;
        }
        resize(sz);
        service = decode_service();
        if (dend == sz) {
            log("comm", "read", "OK", dend, sz, compute_hash());
            #if CFG_COUNTERS==1
                ++counters.valid_dgram_fragment1;
                counters.update(*this, false);
            #endif
            return ok;
        }
        log("Received header for dgram sz=", sz, "svc=", service);
    }
    int nread;
    int tries = 0;
    while(true) {
        busyled.reset();
        nread = ::recv(sock, data() + dend, size() - dend, 0);
        busyled.set();
        log("comm", "read", nread);
        if (unlikely(nread <= 0)) {
            if (errno == EAGAIN || errno == EINPROGRESS) {
                log("recv 0 bytes.", "dend", dend, "errno", errno, strerror(errno), "OK preserving connection");
                return ok;
            }
            if (nread == 0) {
                if (errno == 0) {
                    log("recv 0 bytes.", "dend", dend, "errno", errno, strerror(errno), KO_9021, ".2", "fd", sock);
                    return KO_9021;
                }
            }
            log("comm", "read", nread, "unavailable", "KO 4921.2 errno", errno, "fd", sock);
            return KO_4921;
        }
        break;
    }
    dend += nread;
    #if CFG_COUNTERS == 1
        counters.bytes_received += nread;
        ++counters.valid_dgram_fragment2;
    #endif
    log("comm", "read", dend, size(), completed()?"OK":"ok", completed() ? compute_hash() : hash_t(0));
    #if CFG_LOGS == 1
        if (completed()) {
            log("comm", "recv dgram hash ", compute_hash(), "svc", service, "seq", decode_sequence());
        }
    #endif
    return ok;
}

hash_t c::compute_hash() const {
    hasher_t hasher;
    hasher.write(data(), size());
    hasher_t::value_type v;
    hasher.finalize(v);
    return move(v);
}

hash_t c::compute_hash_payload() const {
    hasher_t hasher;
    hasher.write(data() + h,size() - h);
    hasher_t::value_type v;
    hasher.finalize(v);
    return move(v);
}

void c::dump(ostream& os) const {
    os << "size " << size() << '\n';
    if (dend < h) {
        os << "incomplete\n";
        return;
    }
    os << "service " << decode_service() << '\n';
    os << "payload size " << decode_size() << '\n';
    os << "completed " << completed() << '\n';
}

#if CFG_COUNTERS == 1
    void c::iodata::data::to_stream(ostream& os) const {
        os << num.load() << ' ' << bytes.load() << ' ';
    }

    void c::iodata::data::dump(ostream& os) const {
        os << num.load() << " dgrams " << bytes.load() << " bytes";
    }

    void c::iodata::dump(ostream& os) const {
        os << "sent "; sent.dump(os);
        os << "; recv "; received.dump(os);
    }

    void c::iodata::to_stream(ostream& os) const {
        sent.to_stream(os);
        received.to_stream(os);
    }

    c::counters_t::counters_t() {
        {
            using namespace us::gov::protocol;
            #include <us/api/generated/gov/c++/datagram_counters>
        }
        {
            using namespace us::wallet::protocol;
            #include <us/api/generated/wallet/c++/datagram_counters>
        }
    }

    c::counters_t::~counters_t() {
        clean();
    }

    void c::counters_t::clean() {
        for (auto& i: *this) delete i.second;
        clear();
    }

    void c::counters_t::update(const datagram& d, bool send) {
        auto i = find(d.service);
        if (unlikely(i == end())) return;
        iodata::data* x = send ? &i->second->sent : &i->second->received;
        ++x->num;
        atomic_fetch_add(&x->bytes, (uint32_t)d.size());
    }

    void c::counters_t::to_stream_io(ostream& os) const {
        os << size() << ' ';
        for (auto& i:*this) {
            os << i.first << ' ';
            i.second->to_stream(os);
        }
    }

    void c::counters_t::dump_bw(const chrono::system_clock::time_point& up_since, ostream& os) const {
        using namespace std::chrono;
        auto i = totals();
        auto secs = duration_cast<seconds>(system_clock::now() - up_since).count();
        os << "bandwidth:\n";
        os << " seconds up " << secs << '\n';
        double upavg = (double)i.first/(double)secs;
        double downavg = (double)i.second/(double)secs;
        upavg /= 1000;
        downavg /= 1000;
        os << " upload " << i.first << " bytes\n";
        os << " upload avg " << upavg << " Kbytes/sec\n";
        os << " download " << i.second << " bytes\n";
        os << " download avg " << downavg << " Kbytes/sec\n";
    }

    void c::counters_t::dump(ostream& os) const {
        for (auto& i:*this) {
            os << i.first << ' ';
            i.second->dump(os);
            os << '\n';
        }
        os << "bytes_received " << bytes_received << '\n';
        os << "wrong_channel " << wrong_channel << '\n';
        os << "closed_connection " << closed_connection << '\n';
        os << "peer_timeout " << peer_timeout << '\n';
        os << "peer_ended " << peer_ended << '\n';
        os << "infraheader_datagram " << infraheader_datagram << '\n';
        os << "wrong_size " << wrong_size << '\n';
        os << "valid_dgram_fragment1 " << valid_dgram_fragment1 << '\n';
        os << "valid_dgram_fragment2 " << valid_dgram_fragment2 << '\n';
    }

    pair<uint32_t, uint32_t> c::counters_t::totals() const {
        pair<uint32_t, uint32_t> ans;
        for (auto& i: *this) {
            ans.first += i.second->sent.bytes;
            ans.second += i.second->received.bytes;
        }
        return ans;
    }
#endif

bool c::dispatcher_t::dispatch(datagram* d) {
    assert(d != nullptr);
    log("datagram arrived but was not consumed. svc", d->service);
    return false;
}

