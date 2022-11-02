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
#include "db_analyst.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <filesystem>

#include <us/gov/io/cfg.h>
#include <us/gov/cash/app.h>

#include "db_t.h"
#include "diff.h"
#include "daemon_t.h"
#include "types.h"

#define loglevel "gov/engine"
#define logclass "db_analyst"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::db_analyst;

c::block::block(const hash_t& t): hash_t(t) {
}

c::block::~block() {
    for (auto& i: forks) delete i.second;
}

string c::block::label(const hash_t& head) const {
    static constexpr size_t W = 35;
    string c0 = content();
    ostringstream os0;
    os0 << c0 << ' ' << forks.size() << ' ' << (head == *this ? 'H' : prevh.is_zero() ? 'G' : ' ');
    string c = os0.str();
    int a = w * W - c.size();
    int r = a / 2;
    int l = a - r;
    ostringstream os;
    os << string(l, ' ');
    os << c;
    os << string(r, ' ');
    return os.str();
}

string c::block::content() const {
    ostringstream os;
    os << type << ' ' << *this;
    return os.str();
}

void c::block::dump(int th, const hash_t& head, ostream& os) const {
    if (th == 0) {
        os << label(head);
        return;
    }
    for (auto& i: forks) {
        i.second->dump(th - 1, head, os);
    }
}

void c::block::dump(const string& prefix, ostream& os) const {
    os << prefix << *this << " -> " << prevh << '\n';
    for (auto& i: forks) {
        os << prefix << "fork " << i.first << '\n';
        i.second->dump(prefix + "  ", os);
    }
    os << '\n';
}

void c::block::dot(ostream& os) const {
    if (prevh.is_zero()) {
        os << "\"" << *this << "\" [style=filled color=\"black\" fillcolor=\"yellow\"]\n";
    }
    else {
        os << "\"" << *this << "\" [style=filled color=\"black\" fillcolor=\"lightyellow\"]\n";
        os << "\"" << *this << "\" -> \"" << prevh << "\"\n";
    }
    for (auto& i: forks) {
        i.second->dot(os);
    }
}

c::block* c::block::root() {
    auto d = this;
    while(d->prev != nullptr) d = d->prev;
    return d;
}

c::block* c::block::insert(c::block* b) {
    log("    I am ", (*this), "; prevh=", prevh, "; prev=", prev);
    log("    given block b=", (*b), "; b->prevh=", b->prevh, "; b->prev=", prev);
    if (*b == *this) {
        assert(b->prev == prev);
        log("      hashes match, I return myself");
        return root();
    }
    if (prevh == *b) {
        log("      b is my parent block");
        if (prev != nullptr) {
            assert(prev == b);
        log("         return root() (already linked)");
            return root();
        }
        log("         I add myself to b's fork list");
        b->forks.emplace(*this, this);
        log("         I set b as my parent block");
        prev = b;
        log("         return root()");
        return root();
    }
    if (b->prevh == *this) {
        log("      I am b's parent");
        if (b->prev != nullptr) {
            assert(b->prev = this);
        log("         return me (b has me already linked)");
            return root();
        }
        log("         I add b to my fork list");
        forks.emplace(*b, b);
        log("         I set myself as parent of b");
        b->prev = this;
        log("         return myself");
        return root();
    }
        log("      I have ", forks.size(), " forks");
    for (auto& f: forks) {
        log("      I insert b into one of my forks");
        auto p = f.second->insert(b);
        if(p != 0) {
            log("      inserted succesfully, return myself");
            return root();
        }
    }
    return nullptr;
}

pair<size_t, size_t> c::block::rect() const {
    w = 0;
    h = 0;
    for (auto& i: forks) {
        auto r = i.second->rect();
        w += r.first;
        if (r.second > h) h = r.second;
    }
    ++h;
    if (w == 0) w = 1;
    return make_pair(w, h);
}

c::chains_t::chains_t() {
}

c::chains_t::~chains_t() {
    for (auto& i: roots) delete i;
}

void c::chains_t::dump(const hash_t& head, ostream& os) const {
    vector<vector<string>> m;
    int w = 0, h =0;
    for(auto& i: roots) {
        auto r = i->rect();
        w += r.first;
        if (r.second>h) h = r.second;
    }
    vector<string> lines;
    lines.reserve(h);
    for (int i = 0; i < h; ++i) {
        ostringstream line;
        for(auto& z: roots) {
            z->dump(i, head, line);
        }
        lines.push_back(line.str());
    }
    for (auto& i: lines) {
        os << i << '\n';
    }
}

vector<string> c::chains_t::files(const string& home) {
    namespace fs = std::filesystem;
    vector<string> r;
    for(auto& p: fs::directory_iterator(home)) {
        if (!is_regular_file(p.path())) continue;
        if (p.path().filename() == "head") continue;
        r.push_back(p.path().filename());
    }
    return r;
}

void c::chains_t::load(const string& home) {
    vector<string> f = files(home);
    load(home, f);
}

void c::print_app30db(const string& file, int detail, ostream& os) {
    string homedir = "/tmp/db_analyst";
    engine::daemon_t e(0, crypto::ec::keys::generate(), homedir, 0, 0, 0, 0, 0, vector<hostport_t>(), "");
    auto r = e.load_db(file);
    if (is_ko(r)) {
        os << r << '\n';
        return;
    }
    e.db->cash_app->db.dump("", detail, os);
}

void c::print_app20db(const string& file, ostream& os) {
    string homedir = "/tmp/db_analyst";
    engine::daemon_t e(0, crypto::ec::keys::generate(), homedir, 0, 0, 0, 0, 0, vector<hostport_t>(), "");
    auto r = e.load_db(file);
    if (is_ko(r)) {
        os << r << '\n';
        return;
    }
    e.db->auth_app->db.dump("", os);
}

ko c::read_header_prev(const string& file, blob_reader_t::blob_header_t& header, hash_t& prev) {
    ifstream is(file, ios::binary);
    if (!is.good()) {
        return blob_reader_t::KO_60498;
    }
    is.get(reinterpret_cast<char&>(header.version));
    is.get(reinterpret_cast<char&>(header.serid));
    if (!is.good()) {
        return blob_reader_t::KO_60498;
    }
/*
    if (unlikely(header.version != blob_reader_t::current_version)) {
        log("Blob is not current version.", +header.version, +blob_reader_t::current_version);
        if (header.version == blob_reader_t::current_version - 1) {
            log("Blob is prev version.", +header.version, +blob_reader_t::current_version);
            return ok;
        }
        log(blob_reader_t::KO_60499, +header.version, +blob_reader_t::current_version);
        return blob_reader_t::KO_60499;
    }
*/
    return prev.read(is);
}

void c::print_chain(const string& home, const hash_t& tip, ostream& os) {
    os << home << '\n';
    hash_t cur = tip;
    hash_t prev;
    while (!cur.is_zero()) {
        ostringstream file;
        file << home << '/' << cur;
        blob_reader_t::blob_header_t hdr;
        auto r = read_header_prev(file.str(), hdr, prev);
        if (is_ko(r)) {
            os << r << ' ' << cur << '\n';
            return;
        }
        if (hdr.serid == diff::serid) {
            os << "D " << cur << '\n';
        }
        else if(hdr.serid == engine::db_t::serid) {
            prev.zero();
            os << "S " << cur << '\n';
        }
        cur = prev;
    }
}

void c::chains_t::load(const string& home, const vector<string>& files) {
    int step = 0;
    for(auto& p: files) {
        auto h = hash_t::from_b58(p);
        block* b = new block(h);
        ifstream is(home + "/" + p);
        if (!is.good()) {
            cerr << "corrupt filename 1 " << p << '\n';
            delete b;
            return;
        }
        char version;
        is >> version;
        char type;
        is >> type;
        if (b->type == diff::serid) {
            is >> b->prevh;
            if (!is.good()) {
                cerr << "corrupt filename 2 " << p << '\n';
                delete b;
                return;
            }
        }
        else if (b->type == engine::db_t::serid) {
            b->prevh.zero();
        }
        list<block*>::iterator ip = roots.end();
        for (auto r = roots.begin(); r != roots.end(); ++r) {
            auto p = (*r)->insert(b);
            if (p != 0) {
                ip = r;
                *ip = p;
                break;
            }
        }
        if (ip == roots.end()) {
            roots.push_back(b);
        }
        else {
            bool again = true;
            while(again) {
                again = false;
                for (auto r = roots.begin(); r != roots.end(); ++r) {
                    if (r == ip) {
                        continue;
                    }
                    {
                        auto p = (*ip)->insert(*r);
                        if (p != 0) {
                            *ip = p;
                            again = true;
                            *r = 0;
                            break;
                        }
                    }
                    {
                        auto p=(*r)->insert(*ip);
                        if (p != 0) {
                            *r = p;
                            again = true;
                            *ip = 0;
                            ip = r;
                            break;
                        }
                    }
                }
                for (auto i = roots.begin(); i != roots.end(); ) {
                    if (*i == 0) i = roots.erase(i);
                    else ++i;
                }
            }
        }
    }

    vector<block*> r;
    r.reserve(roots.size());
    for (auto& i: roots) {
        r.push_back(i);
    }
    sort(r.begin(), r.end(), [](block* a, block* b)->bool { return *a < *b; });
    roots.clear();
    for (auto& i: r) {
        roots.push_back(i);
    }
}

void c::chains_t::dump(const string& prefix, ostream& os) const {
    int n = 0;
    for (auto& i: roots) {
        os << "root " << n++ << "/" << roots.size() <<endl;
        i->dump(prefix + "  ", os);
    }
}

void c::chains_t::dot(ostream& os) const {
    os << "digraph D {\n";
    os << "node [shape=box]\n";
    for (auto& i: roots) {
        i->dot(os);
    }
    set<hash_t> rank;
    for (auto& i: roots) {
        rank.insert(*i);
    }
    if (rank.size() > 1) {
        os << "{rank = same; ";
        for (auto& i: roots) {
            os << "\"" << *i << "\";";
        }
        os << "}\n";
    }
    os << "}\n";
}

c::db_analyst(const string& dir): h(dir) {
    load_head();
    load_blocks();
}

void c::load_blocks() {
    chains.load(h);
}

void c::load_head() {
   string filename = h + "/head";
    ifstream is(filename);
    if (is.good()) is >> head;
}

void c::dump(ostream& os) const {
    os << "head " << head << '\n';
    chains.dump(head, os);
}

void c::dot(ostream& os) const {
    chains.dot(os);
}

