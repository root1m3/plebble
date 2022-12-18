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
#include <iostream>
#include <sstream>
#include <cassert>

#include "conch.h"

using namespace std;
using namespace conch;
using c = conch::section;

ostream* c::pos{nullptr};
ostream* c::peos{nullptr};
string c::version;

namespace {
    constexpr int tabpos = 35;
}

c::section(params&& p): pdef(p) {
    constructor();
}

c::section(): pdef({}) {
    constructor();
}

c::section(function<bool(const params&, ostream&)> f): pdef({}), handler(f) {
}

void c::constructor() {
    auto& def=const_cast<params&>(pdef);
    def.emplace_back(flagdef{'h', "help", true, false, "", "Prints this help."});
    handler = [](const params& p, ostream& os) -> bool {
        os << "Not implemented." << endl;
        return false;
    };
}

void c::add(flagdef&& fd) {
    assert(!pdef.empty());
    auto& def = const_cast<params&>(pdef);
    auto it=--(def.rbegin().base());
    def.emplace(it, move(fd));
}

c::~section() {
    for (auto& i: *this) delete i.second;
}

c& c::add_section(cmddef&&a, section*s) {
    s->parent = this;
    s->name = a.name;
    s->desc = a.desc;
    emplace_back(make_pair(a, s));
    return *s;
}

string c::scope() const {
    if (parent == nullptr) return name;
    return parent->scope() + " " + name;
}

string c::fmt_field(const string& x, int w) {
    ostringstream os;
    if (x.size() >= w) {
        os << x << ' ';
        return os.str();
    }
    os << x << string(w, ' ');
    return os.str().substr(0, w);
}

void c::help(const v_t& v) const {
    assert(!v.empty());
    help(v, --v.end());
}

void c::help(const v_t& v, v_t::const_iterator i) const {
    auto& os = *pos;
    params* pp = nullptr;
    for (auto j = v.begin(); j != v.end(); ++j) { //override ancestor page if forced -h
        if (j->second->is_set('h')) {
            i = j;
            break;
        }
        if (j == i) break;
    }
    pp = i->second;
    auto& p = *pp;
    auto* s = lookup(v, i);
    assert(s != nullptr);
    string sc = s->scope();
    os << sc << "  r" << version << '\n';
    os << '\n';
    os << desc << '\n';
    os << '\n';
    if (v.size()>1) {
        os << "context:\n";
        for (auto j = v.begin(); j != i; ++j) {
            if ((*j).second->any_set()) {
                string ind;
                if (!(*j).first.empty()) {
                    ind="  ";
                    os << ind << (*j).first << ":\n";
                }
                (*j).second->dump_set(ind + "  ", os);
            }
        }
        os << '\n';
    }
    os << "usage:\n";
    os << "  " << sc << ' ';
    if (!p.empty()) {
        os << "[flags] ";
    }
    if (!s->empty()) {
        os << "<command> [-h]";
    }
    os << '\n';
    p.dump("", os);
    if (!s->empty()) {
        os << '\n';
        os << "commands:\n";
        for (auto& i: *s) {
            os << "  " << fmt_field(i.first.name, tabpos) << i.first.desc << '\n';
        }
    }
}

void c::set_handler(function<bool(const params&, ostream&)> f) {
    handler=f;
    skip_handler=false;
}

bool c::fillv(const string& mycmd, v_t& v, istream& is) const {
    params* p = new params(pdef, is);
    v.push_back(make_pair(mycmd, p));
    if (!p->ko.empty()) {
        print_error(p->ko);
        help(v);
        return false;
    }
    string cmd;
    is >> cmd;
    auto s = lookup(cmd);
    if (s == nullptr) {
        if (empty()) {
            if (cmd.empty()) return true;
            print_error(string("Unexpected command. '") + cmd + "'.");
            help(v);
            return false;
        }
        print_error(cmd.empty() ? "Missing command." : string("Invalid command '") + cmd + "'.");
        help(v);
        return false;
    }
    return s->fillv(cmd, v, is);
}

bool c::exec(const v_t& v, v_t::const_iterator i) const {
    params* p = i->second;
    if (p->is_set('h')) {
        auto n = i + 1;
        if (n != v.end()) {
            print_error(string("Invoking help caused command '") + n->first + "' to be ignored.");
            help(v, i);
            return false;
        }
        else {
            help(v, i);
            return true;
        }
    }
    if (!skip_handler) {
        ostringstream os;
        auto r = handler(*p, os);
        if (r) {
            *pos << os.str();
        }
        else {
            *peos << "ERR " << os.str() << '\n';
        }
        if (!r) return false;
    }
    ++i;
    if (i == v.end()) return true;
    auto s = lookup(i->first);
    assert(s != nullptr);
    return s->exec(v, i);
}

c::v_t::const_iterator c::check_req(const v_t& v, ostream&os) const {
    auto it = v.end();
    for (auto i = v.begin(); i != v.end(); ++i) {
        if (!i->second->check_req(os)) {
            if (it == v.end()) it = i;
        }
    }
    return it;
}

params* c::v_t::lookup(const vector<string>& cmdpath) {
    if (empty()) return nullptr;
    auto i = begin();
    if (cmdpath.empty()) return i->second;
    ++i;
    auto j = cmdpath.begin();
    while(i != end()) {
        if (i->first != *j) return nullptr;
        ++j;
        if (j == cmdpath.end()) break;
        ++i;
    }
    if (i != end()) {
        return i->second;
    }
    return nullptr;
}

bool c::exec(istream& is) const {
    v_t v;
    auto r = fillv("", v, is);
    if (!r) {
        return false;
    }
    rewrite(v);
    ostringstream os;
    auto it = check_req(v, os);
    if (it != v.end()) {
        print_error(os.str());
        help(v, it);
        return false;
    }
    r = exec(v, v.begin());
    for (auto& i: v) delete i.second;
    return r;
}

const c* c::lookup(const string& cmd) const {
    for (auto i = begin(); i != end(); ++i) {
        if (i->first.name == cmd) return i->second;
    }
    return nullptr;
}

const c* c::root() const {
    auto p = this;
    while(p->parent != nullptr) p = p->parent;
    return p;
}

const c* c::lookup(const v_t& v, v_t::const_iterator t) const {
    assert(t != v.end());
    if (v.empty()) return nullptr;
    auto s = root();
    auto i = v.begin();
    assert(i->first.empty());
    if (v.size() == 1) return s;
    while(i != t) {
        ++i;
        s = s->lookup(i->first);
        if (s == nullptr) return nullptr;
    }
    return s;
}

void c::print_error(const string& msg) {
    auto& os = *c::pos;
    os << "ERR " << msg << "\n\n";
}

//-------------------------------------------------params

params::params(b&& fd): b(fd) {
}

params::params(const params& fd, istream& is): b(fd) {
    while (is.good()) {
        string flag;
        auto g = is.tellg();
        is >> flag;
        if (is.fail()) break;
        if (flag.empty()) break;
        if (flag[0] != '-') {
            is.seekg(g, is.beg);
            break;
        }
        auto f = lookup(flag);
        if (f == nullptr) {
            ko = string("Unrecognized flag '") + flag + "'. ";
            break;
        }
        if (f->req_input) {
            is >> f->value;
            if (is.fail() || f->value.empty()) {
                ko = string("Expected value after '") + flag + "'. ";
                break;
            }
        }
        f->overriden = true;
    }
}

bool params::check_req(ostream& os) const {
    bool r = true;
    for (auto& i: *this) {
        if (!i.optional) {
            if (!i.overriden) {
                os << string("'") + i.name + "' is a required input. ";
                r = false;
            }
        }
    }
    return r;
}

void params::dump(const string& pfx, ostream& os) const {
    os << '\n';
    os << pfx << "flags:\n";
    string prefix = pfx + "  ";
    for (auto& i: *this)
        i.dump(prefix, os);
}

void params::dump_values(const string& pfx, ostream& os) const {
    for (auto& i: *this)
        i.dump_value(pfx, os);
}

bool params::any_set() const {
    for (auto& i: *this) {
        if (i.overriden || !i.value.empty() || !i.optional) {
            return true;
        }
    }
    return false;
}

void params::dump_set(const string& pfx, ostream& os) const {
    for (auto& i: *this)
        if (i.overriden || !i.value.empty() || !i.optional)
            i.dump_value(pfx, os);
}

flagdef* params::lookup(const string& f) {
    for (auto i = begin(); i != end(); ++i) {
        if (*i == f) return &*i;
    }
    return nullptr;
}

const string& params::get(const string& k) const {
    for (auto& i: *this) {
        if (i.name == k) {
            return i.value;
        }
    }
    throw "KO 90291 key doesn't exist.";
}

const string& params::get(char k) const {
    for (auto& i: *this) {
        if (i.short_name == k) return i.value;
    }
    throw "KO 90292 key doesn't exist.";
}

bool params::is_set(char flag) const {
    for (auto& i: *this) {
        if (i.short_name != flag) continue;
        return i.overriden;
    }
    return false;
}

bool params::is_set(const string& flag) const {
    for (auto& i: *this) {
        if (i.name != flag) continue;
        return i.overriden;
    }
    return false;
}

bool params::has(char flag) const {
    for (auto& i: *this) {
        if (i.short_name == flag) return true;
    }
    return false;
}

void params::set_optional(char flag) {
    for (auto& i: *this) {
        if (i.short_name != flag) continue;
        i.optional = true;
        break;
    }
}

void params::set_mandatory(char flag) {
    for (auto& i: *this) {
        if (i.short_name != flag) continue;
        i.optional = false;
        break;
    }
}

//-------------------------------------------------flagdef

void flagdef::dump(const string& pfx, ostream& os0) const {
    ostringstream os;
    if (short_name != ' ') {
        os << '-' << short_name;
        if (!name.empty()) os << ',';
        os << ' ';
    }
    if (!name.empty()) {
        os << "--" << name << ' ';
    }
    if (req_input) {
        if (value.empty()) {
            os << "<value> ";
        }
        else {
            os << "(value='" << value << "') ";
        }
    }
    else {
        if (overriden) {
            os << "(flag is set) ";
        }
    }
    os0 << pfx << section::fmt_field(os.str(), tabpos);
    if (!optional) os0 << "Required. ";
    os0 << desc << '\n';
}

void flagdef::dump_value(const string& pfx, ostream& os) const {
    os << pfx << "--" << name << " = " << (req_input ? value : (overriden ? "(set)" : ""))  << '\n';
}

bool flagdef::operator == (const string& i) const {
    if (i.size() < 2) return false;
    if (i[0] != '-') return false;
    if (i[1] != '-') {
        return (string("-") + short_name) == i;
    }
    return (string("--") + name) == i;
}

bool flagdef::operator == (const flagdef& other) const {
    return short_name == other.short_name && name == other.name;
}

//-------------------------------------------------cmddef

bool cmddef::operator == (const string& i) const {
    return name == i;
}

bool cmddef::operator == (const cmddef& other) const {
    return name == other.name;
}

//-------------------------------------------------std

size_t std::hash<flagdef>::operator() (const flagdef& g) const {
    string s = g.name;
    s += '_';
    s += g.short_name;
    return std::hash<string>{}(s);
}

size_t std::hash<cmddef>::operator() (const cmddef& g) const {
    return std::hash<string>{}(g.name);
}

