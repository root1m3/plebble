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
#include "coder.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <cassert>
#include <fstream>

#include "api_t.h"

using namespace us::apitool;
using c = us::apitool::coder;
using namespace std;

bool c::feedback_enabled = false;

string c::fn_name(const apifun& f, bool side_caller) {
    ostringstream os;
    os << (side_caller ? "call_" : "handle_");
    os << f.name;
    return os.str();
}


void c::feedback(const string& file) {
    if (!feedback_enabled) return;
    static int w = 0;
    cout << '.'; cout.flush();
    if (w++ > 40) {
        w = 4;
        cout << endl << "    ";
    }
}

namespace {

    void mkdir_tree(string sub, string dir) {
        if (sub.length() == 0) return;
        int i = 0;
        for (i; i < sub.length(); i++) {
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
}

void c::write_file_prefix(const api_t& a, ostream& os) const {
    ostringstream path;
    path << "generated/" << lang() << '/' << m.process << '/' << a.name;
    ensure_dir(path.str());
    os << path.str() << '/';
}

void c::write_file_prefix(ostream& os) const {
    ostringstream path;
    path << "generated/" << lang() << '/' << m.process;
    ensure_dir(path.str());
    os << path.str() << '/';
}

void c::sides_prefix(bool side_caller, ostream& os) {
    os << (side_caller ? "cllr_" : "hdlr_");
}

pair<string, string> c::names_in(const apifun& f, bool side_caller) const {
    pair<string, string> r;
    string side = side_caller ? "" : "_dst";
    r.first = f.name + "_in" + side + "_t";
    r.second = "o_in";
    if (f.in.size() == 1) {
        r.first = get_type(f.in.begin()->first);
        r.second = f.in.begin()->second;
    }
    return r;
}

pair<string, string> c::names_out(const apifun& f, bool caller) const {
    pair<string, string> r;
    string side = caller ? "_dst" : "";
    r.first = f.name + "_out" + side + "_t";
    r.second = "o_out";
    if (f.out.size() == 1) {
        r.first = get_type(f.out.begin()->first);
        r.second = f.out.begin()->second;
    }
    return r;
}
void c::gen_purevir(const api_t& a, bool side_caller, ostream& os) const {
    a.info(os);
    for (auto& f: a) {
        gen_purevir(f, side_caller, os);
    }
}

void c::gen_purevir(const api_t& a, bool side_caller) const {
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "purevir";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_purevir(a, side_caller, os);
    a.warn_f(os);
}

void c::gen_purevir(const api_t& a) const {
    gen_purevir(a, false);
    gen_purevir(a, true);
}

void c::gen_override(const api_t& a, bool side_caller, ostream& os) const {
    a.info(os);
    for (auto& i: a) {
        gen_override(i, side_caller, os);
    }
}

void c::gen_override(const api_t& a, bool side_caller) const {
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "override";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_override(a, side_caller, os);
    a.warn_f(os);
}

void c::gen_override(const api_t& a) const {
    gen_override(a, false);
    gen_override(a, true);
}

void c::gen_rpc_impl(const api_t& a, bool side_caller, ostream& os) const {
    for (auto& i: a) {
        if (gen_rpc_impl(i, side_caller, os)) {
            os << '\n';
        }
    }
}

void c::gen_rpc_impl(const api_t& a) const {
    bool side_caller = true;
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "rpc-impl";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_rpc_impl(a, side_caller, os);
    a.warn_f(os);
}

void c::gen_local_impl(const api_t& a, ostream& os) const {
    for (auto& i: a) {
        if (gen_local_impl(i, os)) {
            os << '\n';
        }
    }
}

void c::gen_local_impl(const api_t& a) const {
    bool side_caller = false;
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "local-impl";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.templ_h(file, os);
    gen_local_impl(a, os);
    a.templ_f(os);
}

void c::gen_service_router(const api_t& a, bool side_caller, ostream& os) const {
    a.info(os);
    for (auto& i: a) {
        gen_service_router(i, side_caller, os);
    }
}

void c::gen_service_router(const api_t& a) const {
    bool side_caller = false;
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "svc-router";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_service_router(a, side_caller, os);
    a.warn_f(os);
}

void c::gen_dto_hdr(const api_t& a, bool side_caller, ostream& os) const {
    a.info(os);
    os << '\n';
    for (auto& i: a) {
        gen_dto_in_hdr(i, side_caller, os);
        gen_dto_out_hdr(i, side_caller, os);
    }
}

void c::gen_dto(const api_t& a, bool side_caller) const {
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "dto-hdr";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_dto_hdr(a, side_caller, os);
    a.warn_f(os);
}

void c::gen_dto(const api_t& a) const {
    gen_dto(a, true);
    gen_dto(a, false);
}

void c::do_common_api(const api_t& a) const {
    gen_purevir(a);
    gen_override(a); //c++
    gen_rpc_impl(a);
    gen_local_impl(a);
    gen_service_router(a);
    gen_dto(a);
}

void c::gen_protocol(const api_t& a, int base) const {
    ostringstream fn;
    write_file_prefix(a, fn);
    fn << "svc";
    feedback(fn.str());
    ofstream os(fn.str());
    gen_protocol(a, base, os);
}

void c::gen_service_handlers(const api_t& a, const string& scope, bool side_caller, ostream& os) const {
    a.info(os);
    for (auto& i: a) {
        if (gen_service_handlers(i, scope, side_caller, os)) {
            os << '\n';
        }
        if (i.out.async_handler) {
            if (gen_service_handlers_response(i, scope, side_caller, os)) {
                os << '\n';
            }
        }
    }
}

void c::gen_service_handlers(const api_t& a, const string& scope) const {
    bool side_caller = false;
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "svc_handler-impl";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_service_handlers(a, scope, side_caller, os);
    a.warn_f(os);
}

void c::do_gov_daemon_api(const api_t& a, int base, ostream& counters_include) const {
    do_common_api(a);
    gen_protocol(a, base);
    gen_gov_protocol_counters_init(a, base, counters_include); //c++
    gen_service_handlers(a, "");
    gen_service_handler_headers(a, ""); //c++
}

void c::gen_gov_daemon_api() const {
    ostringstream fn;
    write_file_prefix(fn);
    fn << "datagram_counters";
    string file = fn.str();
    feedback(fn.str());
    ofstream include_os(file);
    api_t::warn_h(include_os);
    for (auto& i: m) {
        do_gov_daemon_api(*i.first, i.second, include_os);
    }
    api_t::warn_f(include_os);
}

void c::generate() const {
    gen_gov_daemon_api();
}

