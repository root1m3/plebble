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
#include "model.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <sys/stat.h>
#include <sys/types.h>

#include "api_t.h"
#include "coder_cpp.h"
#include "coder_java.h"

using namespace std;
using namespace us::apitool;
using c = us::apitool::model;

c::model(const string& process): process(process) {
    load_netsvc_run();
}

c::~model() {
    for (auto& i: *this) {
        delete i;
    }
}

void c::load_netsvc_run() {
    netsvc.netsvc_net.clear();
    netsvc.netsvc_net2.clear();
    ifstream is(string("netsvc/") + process);
    if (is.fail()) {
        netsvc_serial = 0;
        return;
    }
    if (!is.good()) {
        netsvc_serial = 0;
        return;
    }
    string line1;
    string line2;
    string line3;
    string line4;
    getline(is, line1);
    getline(is, line2);
    getline(is, line3);
    getline(is, line4);
    {
        istringstream is2(line1);
        string nm;
        string p;
        is2 >> nm;
        assert(nm == "process");
        is2 >> p;
        if (is2.fail()) {
            netsvc_serial = 0;
            return;
        }
        assert(process == p);
    }
    {
        istringstream is2(line2);
        string nm;
        is2 >> nm;
        assert(nm == "version");
        int n{0};
        is2 >> n;
        netsvc_serial = (uint8_t) n;
        if (is2.fail()) {
            netsvc_serial = 0;
            return;
        }
    }
    while(is.good()) {
        int svc;
        string nme;
        is >> svc;
        if (is.fail()) break;  
        is >> nme;
//cout << "load " << svc << nme << endl;
        if (nme.empty()) continue;
        netsvc.netsvc_net.emplace(svc, nme);
        netsvc.netsvc_net2.emplace(nme, svc);
    }
    netsvc.netsvc_hit = netsvc.netsvc_net2;
}

namespace {

    void mkdir_tree(string sub, string dir) {
        if (sub.length() == 0) return;
        size_t i = 0;
        for (; i < sub.length(); i++) {
            dir += sub[i];
            if (sub[i] == '/')
            break;
        }
        ::mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if ((i + 1) < sub.length()) mkdir_tree(sub.substr(i + 1), dir);
    }

    bool dir_exists(const string& d) {
        struct stat s;
        if (stat(d.c_str(), &s) == 0) return S_ISDIR(s.st_mode);
        return false;
    }

    bool ensure_dir(const string& d) {
        if (!dir_exists(d)) {
            mkdir_tree(d, "");
            if (!dir_exists(d)) {
                cerr << "KO 60983 Creating directory " << d << endl;
                assert(false);
                return false;
            }
        }
        return true;
    }

    string tostring(uint8_t v) {
        ostringstream os;
        os << +v;
        return os.str();
    }
}

void c::write_netsvc(ostream& os) {
    string snetsvc_serial = tostring(netsvc_serial);
    os << "process " << process << '\n';
    os << "version " << +netsvc_serial << '\n';
    os << "service function\n";
    os << "------- --------\n";
    for (auto &i: netsvc.netsvc) {
        os << std::setw(7) << i.first << ' ' << i.second << '\n';
    }
}

int c::add(const string& name, int svc0) {
//cout << "ADD API " << name << endl;
    auto a = api_t::load(process, name, svc0);
//    emplace_back(make_pair(a.first, svc0));
    emplace_back(a.first);
    a.first->m = this;
    return a.second; //a->svc_end(svc0);
}

int c::add_delegate(const string& name, int svc0) {
    auto a = api_t::load(process, name, svc0);
    emplace_back(a.first);
    a.first->m = this;
    a.first->delegate = true;
    return a.second;
}

void c::end_adding() {
    bool ch{false};

    //ostringstream os;
    //ostringstream osinv;

    for (auto& i: *this) {
        i->compute_netsvc(netsvc, ch, svcfish, svcfish_inv);
    }
    if (!netsvc.netsvc_hit.empty()) {
        for (auto& i: netsvc.netsvc_hit) {
            svcfish.emplace_back(svcfish_entry_t{'-', i.first, i.second, "", -1, "svc deleted"});
            svcfish_inv.emplace_back(svcfish_entry_t{'/', "", -1, i.first, i.second, "wont be called"});
//            os << "{ " << i.second << ", -1}, " << " // - {" << i.first << " " << i.second << "}\n";
//            osinv << " // wont be called {" << i.first << " " << i.second << "}\n";
        }
        ch = true;
    }
    prevserial = netsvc_serial;
    if (ch) {
        if (netsvc_serial == 255) {
            netsvc_serial = 0;
        }
        ++netsvc_serial;
    }
//    svcfish = os.str();
//    svcfish_inv = osinv.str();
    
}



