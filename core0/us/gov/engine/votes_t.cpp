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
#include <set>

#include "votes_t.h"

#define loglevel "gov/engine"
#define logclass "votes_t"
#include "logs.inc"

using namespace us::gov::engine;
using c = us::gov::engine::votes_t;

void c::clear() {
    lock_guard<mutex> lock(mx);
    voters.clear();
    votes.clear();
}

bool c::add(const pubkey_t::hash_t& h, const hash_t& v) {
    lock_guard<mutex> lock(mx);
    if (closed) {
        log("new round");
        closed = false;
        voters.clear();
        votes.clear();
    }
    #ifdef CFG_PERMISSIONED_NETWORK
        static pubkey_t::hash_t master_node{"2LCbMR7q5EyxakTdvaKXZzeb4kaH"}; //TODO from conf
        if (h == master_node) {
            master_vote = v;
            log ("set master vote", master_vote, "from master node", h);
        }
    #endif
    auto i = voters.find(h);
    if (i != voters.end()) {
        return false;
    }
    #if CFG_COUNTERS == 1
        voters.emplace(h, v);
    #else
        voters.emplace(h);
    #endif
    auto o = votes.find(v);
    if (o != votes.end()) {
        ++o->second;
    }
    else {
        votes.emplace(v, 1);
    }
    return true;
}

void c::dump(const string& prefix, ostream& os) const {
    auto pfx = prefix + "  ";
    #if CFG_COUNTERS == 1
        if (votes.size() > 1) {
            os << prefix << "voters:\n";
            for (auto& i: voters) {
                os << pfx << i.first << " voted " << i.second << '\n';
            }
        }
    #endif
    os << prefix << voters.size() << " voters\n";
    os << prefix << votes.size() << " beams:\n";
    for (auto& i: votes) {
        os << pfx << i.second << " votes for " << i.first << '\n';
    }
}

#ifdef CFG_PERMISSIONED_NETWORK

hash_t c::select() {
        lock_guard<mutex> lock(mx);
        auto r = master_vote;
        master_vote = 0;
        closed = true;
        log ("selecting master vote",r);
        return r;
}

void c::logline(ostream& os) const {
    os << "MV " << master_vote << ' ';
}

#else

void c::logline(ostream& os) const {
    multiset<int> x;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: votes) {
          x.emplace(i.second);
        }
    }
    os << "{";
    int r = 0;
    int p = 0;
    for (auto i = x.rbegin(); i != x.rend(); ++i) {
        if (p < 2) {
            os << *i << ' ';
            ++p;
        }
        else {
            r += *i;
        }
    }
    if (r > 0) {
        os << "R" << r << " NB " << x.size() << "} ";
    }
    os << "NV " << voters.size() << "} ";
}

hash_t c::select() {
    set<hash_t> x;
    uint64_t n = 0;
    {
        lock_guard<mutex> lock(mx);
        for (auto& i: votes) {
            if (i.second < n) continue;
            if (i.second > n) {
                x.clear();
                n = i.second;
            }
            x.insert(i.first);
        }
        closed = true;
    }
    if (x.empty()) {
        log ("selecting null vote");
        return hash_t(0);
    }
    log ("selecting stronger beam vote", *x.begin());
    return *x.begin();
}

#endif

