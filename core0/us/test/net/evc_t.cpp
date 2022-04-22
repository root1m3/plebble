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
#include "evc_t.h"

#include <us/gov/config.h>

#define loglevel "test/net"
#define logclass "evc_t"
#include <us/gov/logs.inc>

#include "../assert.inc"

using namespace us;
using namespace us::test;
using c = us::test::evc_t;

c::evc_t(int numnodes): numnodes(numnodes) {
}

void c::wait_empty() {
    unique_lock<mutex> lock(mx);
    condition_variable cv;
    while(!empty()) {
        cv.wait_for(lock, 1s, [&] {
//            cout << "evc_t size " << size() << endl;
            return empty();
         });
    }
}

void c::purge_() {
    using namespace std::chrono;
    auto n = chrono::system_clock::now() - seconds(10);
    auto i = begin();
    while(i != end()) {
        if (i->first < n) {
            erase(i);
            i = begin();
        }
        else {
            break;
        }
    }
}

void c::newev(hash_t h, int srcport) {
    auto n = chrono::system_clock::now();
    pld p(h, numnodes);
    lock_guard<mutex> lock(mx);
    emplace(n, p);
    ++total;
    //purge_();
}

void c::recv(const hash_t& h, int ndx) {
    log("recv", h, ndx);
    lock_guard<mutex> lock(mx);
    auto i = begin();
    for (; i != end(); ++i) {
        if (i->second.h == h) break;
    }
    if (i == end()) {
        return;
    }
    i->second.a[ndx]++;
    int co = 0;
    for (auto& j: i->second.a) {
        if (j > 0) ++co;
    }
    if (co == numnodes) {
        ++completed;
        erase(i);
    }

}


//void c::recvu(const hash_t& h, int ndx) {
/*
    lock_guard<mutex> lock(mx);
    auto i=find(h);
    if(i==end()) {
        return;
    }
    i->second[ndx]=true;
    int co=0;
    for (auto& j:i->second) {
        if (j) ++co;
    }
    if (co==numnodes) {
        ++completed;
    }
*/
//}

void c::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << "full broadcast completed " << completed << " / " << total << '\n';
    int n = 0;
    for (auto& i: *this) {
        os << i.second.h << " {";
        for (auto& j: i.second.a) {
            os << j << ' ';
        }
        os << "}\n";
        if (++n > 10) {
            os << "...\n";
            break;
        }

    }
}


