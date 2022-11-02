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
        if (mne == "[extensible_dto]") {
            extensible_dto = true;
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

void c::compute_get_protocol_vector(const string& prefix, vector<pair<string, bool>>& r, int& nextsvc) {
    ostringstream os;
    os << prefix << '_' << name;
    r.emplace_back(make_pair(os.str(), is_sync()));
    service = os.str();
    svc = nextsvc;
    ++nextsvc;
    if (is_sync()) {
        svc_ret = nextsvc;
        ++nextsvc;
    }
}

void c::hitfn(netsvc_t& netsvc, const string& serv) const {
    auto x = netsvc.netsvc_hit.find(serv);
    if (x != netsvc.netsvc_hit.end()) {
        netsvc.netsvc_hit.erase(x);
    }
    if (svc_ret != -1) {
        auto x = netsvc.netsvc_hit.find(serv + "_response");
        if (x != netsvc.netsvc_hit.end()) {
            netsvc.netsvc_hit.erase(x);
        }
    }
} 

void c::compute_netsvc(netsvc_t& netsvc, bool& ch, svcfish_db_t& db, svcfish_db_t& dbinv) {
    ch = false;
    netsvc.netsvc.emplace(svc, service);
    string service_ret;
    if (svc_ret != -1) {
        service_ret = service + "_response";
        netsvc.netsvc.emplace(svc_ret, service_ret);
    }
    auto i = netsvc.netsvc_net2.find(service);
    if (i == netsvc.netsvc_net2.end()) { //new function
        db.emplace_back(svcfish_entry_t{'/', "", -1, service, svc, "New function"});
        dbinv.emplace_back(svcfish_entry_t{'-', service, svc, "", -1, "Removed function"});
//        os << " // + {" << svc << ' ' << service << "}\n";
//        osinv << " { " << svc << ", -1}, " << " // - {" << svc << " " << service << "}\n";
        if (svc_ret != -1) {
            db.emplace_back(svcfish_entry_t{'/', "", -1, service_ret, svc_ret, "New function"});
            dbinv.emplace_back(svcfish_entry_t{'-', service_ret, svc_ret, "", -1, "Removed function"});

//            os << " // + {" << svc_ret << ' ' << service << "_response}\n";
//            osinv << " { " << svc_ret << ", -1}, " << " // - {" << svc_ret << " " << service << "_response}\n";
        }
        ch = true;
        return;
    }
    if (i->second == svc) {
        db.emplace_back(svcfish_entry_t{'/', service, svc, service, svc, "Same function"});
        dbinv.emplace_back(svcfish_entry_t{'/', service, svc, service, svc, "Same function"});

//        os << " // = {" << i->first << ' ' << i->second << "}\n";
//        osinv << " // = {" << i->first << ' ' << i->second << "}\n";
        if (svc_ret != -1) {
            db.emplace_back(svcfish_entry_t{'/', service_ret, svc_ret, service_ret, svc_ret, "Same function"});
            dbinv.emplace_back(svcfish_entry_t{'/', service_ret, svc_ret, service_ret, svc_ret, "Same function"});
//            os << " // = {" << i->first << "_response " << (i->second + 1) << "}\n";
//            osinv << " // = {" << i->first << "_response " << (i->second + 1) << "}\n";
        }
        hitfn(netsvc, service);
        return;
    }
    //same service distinct svc
    int oldsvc = i->second;
    string oldservice = i->first;
    db.emplace_back(svcfish_entry_t{'!', oldservice, oldsvc, service, svc, "svc changed"});
    dbinv.emplace_back(svcfish_entry_t{'!', service, svc, oldservice, oldsvc, "svc changed"});
//    os << "{ " << oldsvc << ", " << svc << "}, " << "    // ! {" << oldservice << '(' << oldsvc << ") -> " << service << '(' << svc << ")}\n";
//    osinv << "{ " << svc << ", " << oldsvc << "}, " << " // ! {" << service << '(' << svc << ") -> " << oldservice << '(' << oldsvc << ")}\n";
    if (svc_ret != -1) {

        db.emplace_back(svcfish_entry_t{'!', oldservice + "_ret", oldsvc + 1, service + "_ret", svc + 1, "svc changed"});
        dbinv.emplace_back(svcfish_entry_t{'!', service + "_ret", svc + 1, oldservice + "_ret", oldsvc + 1, "svc changed"});
//        os << "{ " << (oldsvc + 1) << ", " << (svc + 1) << "}, " << "    // ! {" << oldservice << "_response(" << (oldsvc + 1) << ") -> " << service << "_response(" << (svc + 1) << ")}\n";
//        osinv << "{ " << (svc + 1) << ", " << (oldsvc + 1) << "}, " << " // ! {" << service << "_response(" << (svc +1) << ") -> " << oldservice << "_response(" << (oldsvc + 1) << ")}\n";
    }
    ch = true;
    hitfn(netsvc, oldservice);
}

/*    


    auto i = netsvc.netsvc_net.find(svc);
    if (i == netsvc.netsvc_net.end()) { //new function
        auto j = netsvc.netsvc_net2.find(service);
        int oldsvc = -1;
        string oldservice = "didn't exist";
        if (j != netsvc.netsvc_net2.end()) {
            oldsvc = j->second;
            oldservice = j->first;
            os << "{ " << oldsvc << ", " << svc << "}, " << "    // ! {" << oldservice << '(' << oldsvc << ") -> " << service << '(' << svc << ")}\n";
            osinv << "{ " << svc << ", " << oldsvc << "}, " << " // ! {" << i->second << '(' << i->first << ") <- " << service << '(' << svc << ")}\n";
            auto x = netsvc.netsvc_hit.find(oldsvc);
            if (x != netsvc.netsvc_hit.end()) {
                netsvc.netsvc_hit.erase(x);
            }
        }
        else {
            os << " // + {" << svc << ' ' << service << "}\n";
            osinv << " { " << svc << ", -1}, " << " // - {" << svc << " " << service << "}\n";
        }
        ch = true;
    }
    else {
        if (i->second == service) {
            //cout << "= {" << i->first << ' ' << i->second << "}\n";
            os << " // = {" << i->first << ' ' << i->second << "}\n";
            osinv << " // = {" << i->first << ' ' << i->second << "}\n";
        }
        else {
            auto j = netsvc.netsvc_net2.find(service);
            int oldsvc = -1;
            string oldservice = "didn't_exist";
            if (j != netsvc.netsvc_net2.end()) {
                oldsvc = j->second;
                oldservice = j->first;
                os << "{ " << oldsvc << ", " << svc << "}, " << "    // ! {" << oldservice << '(' << oldsvc << ") -> " << service << '(' << svc << ")}\n";
                osinv << "{ " << svc << ", " << oldsvc << "}, " << " // ! {" << oldservice << '(' << oldsvc << ") <- " << service << '(' << svc << ")}\n";
                ch = true;
            }
//            cout << "! {" << svc << ' ' << i->second << " -> " << service << "}\n";
        }
        auto x = netsvc.netsvc_hit.find(svc);
        if (x != netsvc.netsvc_hit.end()) {
            netsvc.netsvc_hit.erase(x);
        }
    }
    netsvc.netsvc.emplace(svc, service);
    //netsvc.netsvc2.emplace(service, svc);
*/
//}

