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
#include "bookmarks_t.h"
#include <sstream>
#include <chrono>
#include <limits>

#include <us/gov/socket/client.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/io/cfg.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#define loglevel "wallet/trader"
#define logclass "bookmarks"
#include <us/gov/logs.inc>

using c = us::wallet::trader::bookmark_t;
using f = us::wallet::trader::bookmark_info_t;
using s = us::wallet::trader::bookmarks_t;
using namespace us::wallet;
using namespace us::wallet::trader;
using namespace std;
using namespace std::chrono;
using us::ko;


f::bookmark_info_t() {
}

f::bookmark_info_t(string&& label, vector<uint8_t>&& ico): label(move(label)), ico(move(ico)) {
}

f::bookmark_info_t(const bookmark_info_t& other): label(other.label), ico(other.ico) {
}

f::bookmark_info_t(bookmark_info_t&& other): label(move(other.label)), ico(move(other.ico)) {
}

f& f::operator = (const f& other) {
    return *this = f(other);
}

f& f::operator = (f&& other) {
    label = move(other.label);
    ico = move(other.ico);
    return *this;
}

size_t f::blob_size() const {
    return blob_writer_t::blob_size(label) + blob_writer_t::blob_size(ico);
}

void f::to_blob(blob_writer_t& writer) const {
    log("writing bookmark_info_t");
    writer.write(label);
    writer.write(ico);
}

ko f::from_blob(blob_reader_t& reader) {
    log("reading bookmark_info_t");
    {
        auto r = reader.read(label);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = reader.read(ico);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

void f::dump(const string& pfx, ostream& os) const {
    os << pfx;
    os << "label: \"" << label << "\" ";
    os << "icon: [" << ico.size() << " bytes]\n";
}

//----------------------------------------------------------------------------

c::bookmark_t() {
}

c::bookmark_t(qr_t&& ep, bookmark_info_t&& i): qr(move(ep)), b(move(i)) {
}

c::bookmark_t(const qr_t& qr_, bookmark_info_t&& i): qr(qr_), b(move(i)) {
}

c::bookmark_t(const c& other): b(other), qr(other.qr) {
}

c& c::operator = (const c& other) {
    static_cast<b&>(*this) = static_cast<const b&>(other);
    qr = other.qr;
    return *this;
}

size_t c::blob_size() const {
    return qr.blob_size() + b::blob_size();
}

void c::to_blob(blob_writer_t& w) const {
    log("writing bookmark_t");
    w.write(qr);
    b::to_blob(w);
}

ko c::from_blob(blob_reader_t& reader) {
    log("reading bookmark_t");
    {
        auto r = reader.read(qr);
        if (is_ko(r)) {
            return r;
        }
    }
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) {
            return r;
        }
    }
    return ok;
}

void c::dump(const string& pfx, ostream& os) const {
    os << pfx;
    os << "qr: \"";
    qr.to_streamX(os);
    os << "\" ";
    b::dump("", os);
}

//----------------------------------------------------------------------------

size_t s::blob_size() const {
     size_t sz = blob_writer_t::sizet_size(size()); //sizeof(uint32_t);
     for (auto& i: *this) {
        sz += blob_writer_t::blob_size(i.first);
        sz += blob_writer_t::blob_size(i.second);
     }
     return sz;
}

void s::to_blob(blob_writer_t& writer) const {
    log("writing bookmarks_t", size());
    writer.write_sizet(size());
    for (auto& i: *this) {
        log("-----");
        writer.write(i.first);
        writer.write(i.second);
    }
}

ko s::from_blob(blob_reader_t& reader) {
    log("reading bookmarks_t");
    uint64_t sz;
    auto r = reader.read_sizet(sz);
    if (is_ko(r)) return r;
    clear();
    for (size_t i = 0; i < sz; ++i) {
        string key;
        {
            auto r = reader.read(key);
            if (is_ko(r)) return r;
        }
        bookmark_t value;
        {
            auto r = reader.read(value);
            if (is_ko(r)) return r;
        }
        emplace(key, move(value));
    }
    return ok;
}


void s::dump(const string& pfx, ostream& os) const {
    int n = 0;
    for (auto& i: *this) {
        os << pfx << ++n << ": '" << i.first << "': ";
        i.second.dump("", os);
    }
}

s& s::operator += (const s& other) {
    for (auto& i: other) {
        add(i.first, i.second);
    }
    return *this;
}

ko s::add(const bookmarks_t& other) {
    for (auto& i: other) {
        auto r = add(i.first, i.second);
        if (is_ko(r)) return r;
    }
    return ok;
}

ko s::name_check(string& name) const {
    while (find(name) != end()) {
        auto i = name.find_last_of('_');
        if (i == string::npos) {
            name += "_0";
            i = name.find_last_of('_');
        }
        auto g = name.substr(i + 1);
        auto n = atoi(g.c_str());
        auto x = name.substr(0, i);
        ++n;
        ostringstream y;
        y << x << '_' << n;
        name = y.str();
    }
    return ok;
}

ko s::remove(const string& key) {
    auto i = find(key);
    if (i == end()) {
        auto r = "KO 30112 bookmark key found";
        log(r);
        return r;
    }
    erase(i);
    return ok;
}

ko s::add(string name, bookmark_t&& o) {
    auto r = name_check(name);
    if (is_ko(r)) {
        return r;
    }
    emplace(name, move(o));
    log("added bookmark. size", size());
    return ok;
}

ko s::add(string name, const bookmark_t& o) {
    auto r = name_check(name);
    if (is_ko(r)) {
        return r;
    }
    emplace(name, o);
    log("added bookmark. size", size());
    return ok;
}

s::const_iterator s::find_protocol_role(const string& prot, const string& role) const {
    for (auto i = begin(); i != end(); ++i) {
        auto& qr = i->second.qr;
        if (qr.protocol_selection.first != prot) continue;
        if (!role.empty()) {
            if (qr.protocol_selection.second != role) continue;
        }
        return i;
    }
    return end();
}

