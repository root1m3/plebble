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
#include "apifun.h"
#include <sstream>
#include <cassert>

using c = us::apitool::apifun;
using namespace us::apitool;
using namespace std;

string c::io_types_t::id() const {
    ostringstream os;
    if (empty()) {
        os << "0";
    }
    else {
        for (auto& i: *this) {
            os << i.first;
        }
    }
    return os.str();
}

string c::io_types_t::identifierX() const {
    return identifierX(*this);
}

string c::io_types_t::identifierX(const vector<pair<string, string>>& v) {
    ostringstream os;
    if (v.empty()) {
        os << "0";
    }
    else {
        for (auto& i: v) {
            os << i.first;
        }
    }
    return identifierX(os.str());
}

string c::io_types_t::identifierX(const string& s0) {
    string s = s0;
    char* p = const_cast<char*>(s.c_str());
    for (size_t i = 0; i < s.size(); ++i) {
        if (*p == '{') { *p = '_'; }
        else if (*p == '}') { *p = '_'; }
        ++p;
    }
    return s;
}

namespace {
    void trim(string& s) {
        if (s.empty()) return;
        char* b = s.data();
        char* e = b + s.size();
        while(b < e) {
            if (*b > ' ') break;
            ++b;
        }
        --e;
        while(b <= e) {
            if (*e > ' ') break;
            --e;
        }
        ++e;
        s = string(b, e - b);
    }
}

bool c::io_types_t::from_stream(istream& is) {
    clear();
    string line;
    getline(is, line);
    trim(line);
    if (line == "-") {
        return true;
    }
    if (line == "[peerinfo]") {
        peerinfo = true;
        return true;
    }
    if (line == "[dgram]") {
        pass_dgram = true;
        return true;
    }
    istringstream l(line);
    while(l.good()) {
        string mne;
        l >> mne;
        if (mne == "[peerinfo]") {
            peerinfo = true;
            continue;
        }
        if (mne == "[dgram]") {
            pass_dgram = true;
            continue;
        }
        if (mne == "[async_handler]") {
            async_handler = true;
            continue;
        }
        if (mne == "[heap]") {
            heap = true;
            continue;
        }
        string argname;
        l >> argname;
        if (!mne.empty() && !argname.empty()) {
            push_back(make_pair(mne, argname));
        }
        else {
            return false;
        }
    }
    return true;
}

c c::example1() {
    c r;
    //int n;
    r.name = "f1";
    r.sync_type = "async";
    return r;
}

c c::from_stream(istream& is) {
    c r;
    //int n;
    {
        string spec;
        getline(is, spec);
        istringstream isp(spec);
        isp >> r.name;
        string oldsuffix;
        isp >> oldsuffix;
    }
    if (r.name.empty()) {
        return r;
    }
    if (!r.in.from_stream(is)) {
        cerr << "in: " << r.name << endl;
        assert(false);
    }
    {
        string syncline;
        getline(is, syncline);
        istringstream is2(syncline);
        is2 >> r.sync_type;
    }
    assert(r.sync_type == "sync" || r.sync_type == "async");
    if (r.sync_type == "sync") {
        if (!r.out.from_stream(is)) {
            cerr << "out: " << r.name << endl;
            assert(false);
        }
    }
    getline(is, r.fcgi);
    getline(is, r.custom_rpc_impl);
r.custom_rpc_impl = "-";
    string optional;
    while(true) {
        getline(is, optional);
        if (optional.empty()) break;
        istringstream is(optional);
        string k;
        is >> k;
        if (k == "--api_stream-write_fn_sfx") {
            is >> r._api_stream_write_fn_sfx;
            continue;
        }
        break;
    }
    return r;
}

void c::collect_in_specs(map<string, vector<pair<string, string>>>& s) const {
    string key = in.id();
    assert(!key.empty());
    while(true) {
        auto i = s.find(key);
        if (i != s.end()) break;
        s.emplace(key, in);
        break;
    }
}

void c::collect_out_specs(map<string, vector<pair<string, string>>>& s) const {
    if (!is_sync()) return;
    string key = out.id();
    assert(!key.empty());
    while(true) {
        auto i = s.find(key);
        if (i != s.end()) break;
        s.emplace(key, out);
        break;
    }
}

void c::compute_get_protocol_vector(const string& prefix, vector<pair<string, bool>>& r) {
    ostringstream os;
    os << prefix << '_' << name;
    r.emplace_back(make_pair(os.str(), is_sync()));
    service = os.str();
}


