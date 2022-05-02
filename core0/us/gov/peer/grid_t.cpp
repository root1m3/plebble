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
#include "grid_t.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <limits>
#include <random>

#include <us/gov/io/cfg.h>

#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/peer"
#define logclass "grid_t"
#include "logs.inc"

using namespace us::gov::peer;
using c = us::gov::peer::grid_t;

void c::grid_t::faillog_t::add(const hostport_t& hostport) {
    ostringstream os;
    os << peer_t::endpoint(hostport);
    lock_guard<mutex> lock(mx);
    push_back(os.str());
    while(size() > 10) {
        pop_front();
    }
}

void c::grid_t::faillog_t::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    if (!empty()) os << "last failed attempts:\n";
    for (auto& i: *this) {
        os << i << '\n';
    }
}

int c::grid_t::num_edges_minage(int secs_old) const {
    lock_guard<mutex> lock(mx_);
    auto now = chrono::system_clock::now();
    int n = 0;
    for (auto& i: *this) {
        if (i == nullptr) continue;
        if (duration_cast<chrono::seconds>(now - i->since).count() > secs_old) {
            ++n;
        }
    }
    return n;
}

bool c::grid_t::find(const hash_t& pk) const {
    for (auto& i: *this) {
        if (i == nullptr) continue;
        if (static_cast<id::peer_t*>(i)->pubkey.hash() == pk) return true;
    }
    return false;
}

peer_t* c::grid_t::prepare_worker_send(unordered_set<const peer_t*>& visited) {
    log("prepare_worker_send", "visited", visited.size());
    lock_guard<mutex> lock(mx_);
    iterator ipw = begin();
    auto n = size();
    log("grid size", n);
    if (unlikely(n == 0)) {
        return nullptr;
    }
    static mt19937_64 rng(random_device{}());
    uniform_int_distribution<> d(0, n - 1);
    advance(ipw, d(rng));
    for (int k = 0; k < n; ++k) {
        ++ipw;
        if (ipw == end()) ipw = begin();
        if (*ipw == nullptr) {
            log("slot", distance(begin(), ipw), "is empty");
            continue;
        }
        auto p = visited.find(*ipw);
        if (p != visited.end()) {
            log("slot occupied", distance(begin(), ipw), "already visited");
            continue;
        }
        visited.emplace(*ipw);
        log("returning slot", distance(begin(), ipw));
        return *ipw;
    }
    log("no suitable peer found in grid");
    return nullptr;
}

peer::peer_t* c::grid_t::pick_one() {
    lock_guard<mutex> lock(mx_);
    auto ipw = cbegin();
    auto n = size();
    if (n == 0) return nullptr;
    static mt19937_64 rng(random_device{}());
    uniform_int_distribution<> d(0, n - 1);
    advance(ipw, d(rng));
    for (int k = 0; k < n; ++k) {
        ++ipw;
        if (ipw == cend()) ipw = cbegin();
        if (*ipw == nullptr) continue;
        ++(*ipw)->sendref; //block gc
        return static_cast<peer_t*>(*ipw);
    }
    return nullptr;
}

namespace {

    template <typename Container, typename Fun>
    void tuple_for_each(const Container& c, Fun fun) {
        for (auto& e: c)
            fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
    }

    string duration_str(chrono::milliseconds time) {
        using namespace std::chrono;
        using T = std::tuple<milliseconds, int, const char *>;
        constexpr T formats[] = {
            T{hours(1), 2, ""},
            T{minutes(1), 2, ":"},
            T{seconds(1), 2, ":"},
        };
        std::ostringstream o;
        tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
            o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
            time = time % denominator;
        });
        return o.str();
    }

    string age(chrono::system_clock::time_point time) {
        using namespace std::chrono;
        return duration_str(duration_cast<milliseconds>(system_clock::now() - time));
    }
}

void c::grid_t::watch(ostream& os) const {
    {
        lock_guard<mutex> lock(mx_);
        int j = 0;
        for (auto i = begin(); i != end(); ++i) {
            if (i == bi) os << '>'; else os << ' ';
            os << "[" << j++ << " ";
            if ((*i) == 0) {
                os << "--:--:-- ---.---.-.--:----- -]";
            }
            else {
                os << age((*i)->since);
                os << ' ' << (*i)->endpoint() << "]";
            }
            if (j % 5 == 0) os << '\n';
        }
        os << '\n';
    }
    faillog.dump(os);
}

void c::grid_t::dump(ostream& os) const {
    lock_guard<mutex> lock(mx_);
    os << "grid size: " << size() << " slots.\n";
    int j = 0;
    for (auto i = begin(); i != end(); ++i) {
        if (i == bi) os << '>'; else os << ' ';
        os << "edge #" << j << " client* " << *i  << '\n';
        ostringstream pfx;
        pfx << " #" << j++ << ">";
        if (*i == 0) {
            os << pfx.str() << "empty\n";
        }
        else {
            log("dumpof", *i);
            try {
                (*i)->dump_all(pfx.str(), os);
            }
            catch(exception e) {
                os << "X";
            }
        }
        os << '\n';
    }
}

bool c::grid_t::ended(peer_t* p) {
    log("rm_from_grid", p, p->endpoint());
    lock_guard<mutex> lock(mx_);
    for (auto i = begin(); i != end(); ++i) {
        if ((*i) == p) {
            log("grid-ended peer_t", p, p->endpoint());
            (*i) = nullptr;
            return true;
        }
    }
    return false;
}

bool c::grid_t::add(peer_t& p, bool check_unique) {
    log("add_to_grid", &p, p.endpoint());
    lock_guard<mutex> lock(mx_);
    if (check_unique) {
        for (auto& i: *this) {
            if (i == nullptr) continue;
            if (i->pubkey == p.pubkey) {
                log("grid same pubkey existing peer_t", &p, i->endpoint(), static_cast<id::peer_t*>(&p)->pubkey.hash());
                i->disconnect(0, "Replaced with another connection with the same pubkey.");
                i = &p;
                log("grid-replaced peer_t with", &p, p.endpoint(), p.pubkey.hash());
                return true;
            }
        }
    }
    ts_t older_ts = numeric_limits<ts_t>::max();
    size_t older = numeric_limits<size_t>::max();
    size_t n = 0;
    for (auto& i: *this) {
        if (i == nullptr) {
            i = &p;
            log("grid-added peer_t", &p, p.endpoint(), p.pubkey.hash());
            return true;
        }
        auto ts = i->activity.load();
        if (ts < older_ts) {
            older_ts = ts;
            older = n;
        }
        ++n;
    }
    log("WP 58410 Grid is full.", check_unique, "grid size=", size());
    auto& o = (*this)[older];
    o->disconnect(0, "Recycled slot.");
    o = &p;
    return true;
}

