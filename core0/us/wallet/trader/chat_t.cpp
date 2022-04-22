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
#include "chat_t.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <us/gov/socket/client.h>
#include <us/gov/crypto/base58.h>
#include "types.h"

#define loglevel "trader"
#define logclass "chat"
#include <us/gov/logs.inc>

using namespace us::wallet::trader;
using c = us::wallet::trader::chat_t;

bool c::paragraphs::find(const string& pattern) const {
    for (auto i = rbegin(); i != rend(); ++i) {
        if (i->find(pattern) != string::npos) return true;
    }
    return false;
}

size_t c::entry::blob_size() const {
    if (unlikely(empty())) return b::blob_size();
    return blob_writer_t::blob_size(me) + b::blob_size();
}

void c::entry::to_blob(blob_writer_t& writer) const {
    b::to_blob(writer);
    if (unlikely(empty())) {
        return;
    }
    writer.write(me);
}

ko c::entry::from_blob(blob_reader_t& reader) {
    {
        auto r = b::from_blob(reader);
        if (is_ko(r)) return r;
    }
    if (unlikely(empty())) return ok;
    {
        auto r = reader.read(me);
        if (is_ko(r)) return r;
    }
    return ok;
}

string c::formatts(ts_t ts) {
    time_t t = ts / 1e9;
    struct tm lt;
    (void) localtime_r(&t, &lt);
    char res[64];
    if (strftime(res, sizeof(res), "%a %b %d %Y %T", &lt) == 0) {
        return "?";
    }
    return res;
}

c::entry::chat_entry(): me(true) {
}

c::entry::chat_entry(const chat_entry& other): b(other), me(other.me) {
}

c::entry::chat_entry(const paragraph& s): me(true) {
    emplace_back(s);
}

c::entry::chat_entry(const paragraphs& s): b(s), me(true) {
}

void c::entry::add(const string& p) {
    emplace_back(p);
}

void c::entry::add(const paragraphs& p) {
    for (auto& i: p) {
        emplace_back(i);
    }
}

void c::entry::dump(const string& pfx, ostream& os) const {
    if (me) {
        os << pfx << "me:\n";
        for (auto& i: *this) {
            os << pfx << i << '\n';
        }
    }
    else {
        os << pfx << "peer:\n";
        string indent = pfx + "                   ";
        for (auto& i: *this) {
            os << indent << i << '\n';
        }
    }
}

string c::last_thing() const {
    if (empty()) return "";
    auto i = rbegin();
    while (i != rend()) {
        if (!i->second.me) {
            return *(i->second.rbegin());
        }
        ++i;
    }
    return "";
}

string c::last_thing_me() const {
    if (empty()) return "";
    auto i = rbegin();
    while (i != rend()) {
        if (i->second.me) {
            return *(i->second.rbegin());
        }
        ++i;
    }
    return "";
}

const c::entry* c::last_entry() const {
    if (empty()) return nullptr;
    auto i = rbegin();
    while (i != rend()) {
        if (!i->second.me) {
            return &i->second;
        }
        ++i;
    }
    return nullptr;
}

const c::entry* c::last_entry_me() const {
    if (empty()) return nullptr;
    auto i = rbegin();
    while (i != rend()) {
        if (i->second.me) {
            return &i->second;
        }
        ++i;
    }
    return nullptr;
}

bool c::empty_me_() const {
    for (auto& i: *this) {
        if (i.second.me) return false;
    }
    return true;
}

uint8_t c::num_me_() const {
    uint8_t x = 0;
    for (auto& i: *this) {
        if (i.second.me) {
            ++x;
            if (x==255) break;
        }
    }
    return x;
}

bool c::empty_peer_() const {
    for (auto&i:*this) {
        if (!i.second.me) return false;
    }
    return true;
}

uint8_t c::num_peer_() const {
    uint8_t x = 0;
    for (auto& i: *this) {
        if (!i.second.me) {
            ++x;
            if (x == 255) break;
        }
    }
    return x;
}

bool c::need_reply() const {
    if (empty()) return false;
    if (rbegin()->second.me) return false;
    return true;
}

bool c::entry::is_blank() const {
    if (empty()) return true;
    for (auto& i: *this) {
        if (!i.empty()) return false;
    }
    return true;
}

void c::emplacexx(ts_t ts, const chat_entry& n) {
    lock_guard<mutex> lock(mx);
    b::emplace(ts, n);
}

void c::emplace_errorxx(ts_t ts, const chat_entry& n, const string& errmsg) {
    chat_entry copy = n;
    copy.emplace_back(string("[[ ") + errmsg + " ]]");
    {
        lock_guard<mutex> lock(mx);
        b::emplace(ts, copy);
    }
}

bool c::clearxx() {
    {
        lock_guard<mutex> lock(mx);
        if (empty()) return false;
        b::clear();
    }
    return true;
}

bool c::is_empty() const {
    lock_guard<mutex> lock(mx);
    return b::empty();
}

void c::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    dump_(os);
}

void c::dump_(ostream& os) const {
    auto i = begin();
    for (; i != end(); ++i) {
        os << formatts(i->first) << ' ';
        i->second.dump("  ", os);
        os << '\n';
    }
}

void c::dump(const string& pfx, ostream& os) const {
    lock_guard<mutex> lock(mx);
    auto i = begin();
    string indent = pfx + "  ";
    for (;i != end(); ++i) {
        os << pfx << formatts(i->first) << ' ';
        i->second.dump(indent, os);
        os << '\n';
    }
}

